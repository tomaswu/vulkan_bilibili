#include "render.hpp"
#include <SDL3/SDL_vulkan.h>
#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <SDL3/SDL.h>
#include <fstream>

Render::Render(SDL_Window *window)
{
    window_ = window;
    auto app_info = vk::ApplicationInfo();
    app_info.setApiVersion(VK_API_VERSION_1_3);
    auto create_info = vk::InstanceCreateInfo();
    create_info.setPApplicationInfo(&app_info);

    // 打开验证层
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    create_info.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()));
    create_info.setPpEnabledLayerNames(validationLayers.data());


    unsigned int count{0};
    std::vector<const char*> extension_names;
    auto extensions = SDL_Vulkan_GetInstanceExtensions(&count);
    for(uint32_t i=0;i<count;i++){
        std::cout << "extension: " << extensions[i] << std::endl;
        extension_names.push_back(extensions[i]);
    }

    create_info.setEnabledExtensionCount(extension_names.size())
               .setPpEnabledExtensionNames(extension_names.data());

    instance_ = vk::createInstance(create_info);
    
    VkSurfaceKHR c_surface;
    auto ret = SDL_Vulkan_CreateSurface(window_, instance_, NULL, &c_surface);
    if(!ret){
        std::cout << "Failed to create Vulkan surface" << SDL_GetError() << std::endl;
        throw std::runtime_error("Failed to create Vulkan surface");
    }
    surface_ = c_surface;

    // 获取物理设备
    auto physical_devices = instance_.enumeratePhysicalDevices();
    for (const auto& device : physical_devices) {
        auto properties = device.getProperties();
        std::cout << "Device: " << properties.deviceName << std::endl;
    }
    if (physical_devices.empty()) {
        throw std::runtime_error("No Vulkan compatible GPU found");
    }

    // 选择物理设备
    physical_device_ = physical_devices[0];

    // 获取设备属性
    auto properties = physical_device_.getProperties();

    queryQueueFamilyIndices();

    // 创建逻辑设备
    vk::DeviceCreateInfo device_cerate_info;
    std::vector<vk::DeviceQueueCreateInfo> device_queue_create_infos;
    float queue_priority = 1.0f;

    if (queue_family_indices.graphics_queue.value()==queue_family_indices.present_queue.value()){
        vk::DeviceQueueCreateInfo device_queue_create_info;
        device_queue_create_info.setPQueuePriorities(&queue_priority)
        .setQueueCount(1)
        .setQueueFamilyIndex(queue_family_indices.graphics_queue.value());
        device_queue_create_infos.push_back(device_queue_create_info);
    }
    else{
        vk::DeviceQueueCreateInfo present_queue_create_info;
        present_queue_create_info.setPQueuePriorities(&queue_priority)
        .setQueueCount(1)
        .setQueueFamilyIndex(queue_family_indices.present_queue.value());
        device_queue_create_infos.push_back(present_queue_create_info);

        vk::DeviceQueueCreateInfo device_queue_create_info;
        device_queue_create_info.setPQueuePriorities(&queue_priority)
        .setQueueCount(1)
        .setQueueFamilyIndex(queue_family_indices.graphics_queue.value());
        device_queue_create_infos.push_back(device_queue_create_info); 
    }

    device_cerate_info.setQueueCreateInfos(device_queue_create_infos);
    
    // Enable swapchain extension
    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    device_cerate_info.setEnabledExtensionCount(static_cast<uint32_t>(deviceExtensions.size()));
    device_cerate_info.setPpEnabledExtensionNames(deviceExtensions.data());
    
    device_ = physical_device_.createDevice(device_cerate_info);

    getQueues();

    // 创建交换链
    int width, height;
    SDL_GetWindowSize(window_, &width, &height);
    querySwapchainInfo(width, height);
    vk::SwapchainCreateInfoKHR swapchain_create_info;
    swapchain_create_info.setClipped(true)
                         .setImageArrayLayers(1)
                         .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
                         .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
                         .setSurface(surface_)
                         .setImageColorSpace(swapchain_info.image_format.colorSpace)
                         .setImageFormat(swapchain_info.image_format.format)
                         .setImageExtent(swapchain_info.extent)
                         .setMinImageCount(swapchain_info.image_count)
                         .setPresentMode(swapchain_info.present_mode);
    if(queue_family_indices.graphics_queue.value()==queue_family_indices.present_queue.value()){
        swapchain_create_info.setQueueFamilyIndices(queue_family_indices.graphics_queue.value())
                             .setImageSharingMode(vk::SharingMode::eExclusive);
    }
    else{
        std::array indices = {queue_family_indices.graphics_queue.value(), queue_family_indices.present_queue.value()};
        swapchain_create_info.setQueueFamilyIndices(indices)
                             .setImageSharingMode(vk::SharingMode::eConcurrent);
    }

    swapchain_ = device_.createSwapchainKHR(swapchain_create_info);

    createImageViews();
    createShaderModules();
    createPipelineLayout();
    createRenderPass();
    createFramebuffers();
    createPipeline();
    createCommandPool();
    createFence();
}

Render::~Render()
{
    // 等待设备空闲，确保所有操作完成
    device_.waitIdle();
    
    device_.destroySemaphore(image_available_semaphore);
    device_.destroySemaphore(render_finished_semaphore);
    device_.destroyFence(cmd_avaliable_fence);
    device_.freeCommandBuffers(command_pool, command_buffer);
    device_.destroyCommandPool(command_pool);
    device_.destroyPipeline(pipeline);
    for(auto& framebuffer:framebuffers){
        device_.destroyFramebuffer(framebuffer);
    }
    device_.destroyRenderPass(render_pass);
    device_.destroyPipelineLayout(layout);
    device_.destroyShaderModule(vert_shader_module);
    device_.destroyShaderModule(frag_shader_module);
    for(auto& image_view:image_views){
        device_.destroyImageView(image_view);
    }
    device_.destroySwapchainKHR(swapchain_);
    device_.destroy();
    instance_.destroySurfaceKHR(surface_);
    instance_.destroy();
}

void Render::render()
{
    int width,height;
    SDL_GetWindowSize(window_, &width, &height);
    auto res = device_.acquireNextImageKHR(swapchain_, UINT64_MAX, image_available_semaphore, nullptr);

    if (res.result!=vk::Result::eSuccess){
        std::cout<<"acquire next image khr error"<<std::endl;
    }

    auto image_index = res.value;
    command_buffer.reset();
    vk::CommandBufferBeginInfo begin;
    begin.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    command_buffer.begin(begin);
    vk::RenderPassBeginInfo renderpass_begin;
    vk::Rect2D render_area;
    render_area.setOffset({0,0})
               .setExtent(swapchain_info.extent);
    vk::ClearValue clear_value;
    clear_value.color = vk::ClearColorValue(std::array<float,4>{0.1f,0.1f,0.1f,1.0f});
    renderpass_begin.setRenderPass(this->render_pass)
                    .setRenderArea(render_area)
                    .setFramebuffer(this->framebuffers[image_index])
                    .setClearValues(clear_value);
    command_buffer.beginRenderPass(renderpass_begin,{});
    command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics,pipeline);
    command_buffer.draw(3,1,0,0);
    command_buffer.endRenderPass();
    command_buffer.end();

    vk::SubmitInfo submit_info;
    // 添加信号量同步
    vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submit_info.setWaitSemaphores(image_available_semaphore)
               .setWaitDstStageMask(wait_stage)
               .setSignalSemaphores(render_finished_semaphore)
               .setCommandBuffers(command_buffer);
    graphics_queue.submit(submit_info,cmd_avaliable_fence);

    if(device_.waitForFences(cmd_avaliable_fence,true,UINT64_MAX)!=vk::Result::eSuccess){
        std::cout<<"wait fence error"<<std::endl;
    }

    vk::PresentInfoKHR present;
    present.setWaitSemaphores(render_finished_semaphore)
           .setImageIndices(image_index)
           .setSwapchains(swapchain_);
    auto ret = present_queue.presentKHR(present);
    if(ret!=vk::Result::eSuccess){
        std::cout<<"present error"<<std::endl;
    }
    device_.resetFences(cmd_avaliable_fence);
}

void Render::querySwapchainInfo(int width, int height)
{
    auto formats = physical_device_.getSurfaceFormatsKHR(surface_);
    swapchain_info.image_format = formats[0];
    for(const auto& format:formats){
        if(format.format==vk::Format::eR8G8B8A8Srgb 
        && format.colorSpace==vk::ColorSpaceKHR::eSrgbNonlinear){
            swapchain_info.image_format = format;
            break;
        }
    }
    auto capabilities = physical_device_.getSurfaceCapabilitiesKHR(surface_);
    swapchain_info.image_count = std::clamp<uint32_t>(2, capabilities.minImageCount, capabilities.maxImageCount);
    swapchain_info.extent.width = std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    swapchain_info.extent.height = std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    swapchain_info.transform = capabilities.currentTransform;

    auto present_modes = physical_device_.getSurfacePresentModesKHR(surface_);
    swapchain_info.present_mode = vk::PresentModeKHR::eFifo;
    for(const auto& mode:present_modes){
        if(mode==vk::PresentModeKHR::eMailbox){
            swapchain_info.present_mode = mode;
            break;
        }
    }
}

void Render::queryQueueFamilyIndices()
{
    auto queue_family_properties = physical_device_.getQueueFamilyProperties();
    for (int i=0;i<queue_family_properties.size();i++) 
    {
        auto &p = queue_family_properties[i];
        if(p.queueFlags & vk::QueueFlagBits::eGraphics){
            queue_family_indices.graphics_queue = i;
        }
        if (physical_device_.getSurfaceSupportKHR(i, surface_)){
            queue_family_indices.present_queue = i;
        }
        if (queue_family_indices){
            break;
        }
    }
}

void Render::getQueues()
{
    graphics_queue = device_.getQueue(queue_family_indices.graphics_queue.value(), 0);
    present_queue = device_.getQueue(queue_family_indices.present_queue.value(), 0);
}

void Render::createImageViews()
{
    auto images = device_.getSwapchainImagesKHR(swapchain_);
    image_views.resize(images.size());
    for(size_t i=0;i<images.size();i++){
        vk::ImageViewCreateInfo image_view_create_info;
        vk::ComponentMapping component_mapping;

        vk::ImageSubresourceRange subresource_range;

        subresource_range.setBaseMipLevel(0)
                         .setLevelCount(1)
                         .setBaseArrayLayer(0)
                         .setLayerCount(1)
                         .setAspectMask(vk::ImageAspectFlagBits::eColor);

        image_view_create_info.setImage(images[i])
                              .setViewType(vk::ImageViewType::e2D)
                              .setFormat(swapchain_info.image_format.format)
                              .setComponents(component_mapping)
                              .setSubresourceRange(subresource_range);
        image_views[i] = device_.createImageView(image_view_create_info);
    }
}

void Render::createFramebuffers()
{
    int width, height;
    SDL_GetWindowSize(window_, &width, &height);
    framebuffers.resize(image_views.size());
    for(size_t i=0;i<image_views.size();i++){
        vk::FramebufferCreateInfo framebuffer_create_info;
        framebuffer_create_info.setRenderPass(render_pass)
                               .setWidth(width)
                               .setHeight(height)
                               .setAttachments(image_views[i])
                               .setLayers(1);
        framebuffers[i] = device_.createFramebuffer(framebuffer_create_info);
    }    
}

void Render::createShaderModules()
{

    auto readSource = [](const std::string& path)->std::string{
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        if(!file.is_open()){
            throw std::runtime_error("failed to open file!");
        }
        auto size = file.tellg();
        std::string buffer(size,'\0');
        file.seekg(0);
        file.read(buffer.data(),size);
        file.close();
        return buffer;
    };

    vk::ShaderModuleCreateInfo vert_create_info;
    std::string vert_sources_code = readSource("../shader/render.vert.spv");
    vert_create_info.setCodeSize(vert_sources_code.size())
                    .setPCode((uint32_t*)(vert_sources_code.data()));
    vert_shader_module = device_.createShaderModule(vert_create_info);

    vk::ShaderModuleCreateInfo frag_create_info;
    std::string frag_sources_code = readSource("../shader/render.frag.spv");
    frag_create_info.setCodeSize(frag_sources_code.size())
                    .setPCode((uint32_t*)(frag_sources_code.data()));
    frag_shader_module = device_.createShaderModule(frag_create_info);
}

void Render::createPipeline()
{
    vk::GraphicsPipelineCreateInfo pipeline_create_info;

    // 1. vertex input
    vk::PipelineVertexInputStateCreateInfo state_create_info;
    pipeline_create_info.setPVertexInputState(&state_create_info);

    // 2. vertex assembly
    vk::PipelineInputAssemblyStateCreateInfo assembly_create_info;
    assembly_create_info.setPrimitiveRestartEnable(false) // 是否启用顶点重绘
                        .setTopology(vk::PrimitiveTopology::eTriangleList); // 连线方式 目前选择每三个点会连成三角形
    pipeline_create_info.setPInputAssemblyState(&assembly_create_info);

    // 3. shader
    shader_stages.resize(2);
    shader_stages[0].setStage(vk::ShaderStageFlagBits::eVertex)
                                 .setModule(vert_shader_module)
                                 .setPName("main");
    shader_stages[1].setStage(vk::ShaderStageFlagBits::eFragment)
                                 .setModule(frag_shader_module)
                                 .setPName("main");
    
    pipeline_create_info.setStages(shader_stages);

    // 4. viewport
    vk::PipelineViewportStateCreateInfo viewport_state_create_info;
    int width, height;
    SDL_GetWindowSize(window_, &width, &height);
    vk::Viewport viewport(0.,0.,width,height,0.,1.);
    vk::Rect2D scissor(vk::Offset2D(0, 0), vk::Extent2D(static_cast<uint32_t>(width), static_cast<uint32_t>(height)));
    viewport_state_create_info.setViewports(viewport)
                        .setScissors(scissor);
    pipeline_create_info.setPViewportState(&viewport_state_create_info);

    // 5. rasterizer
    vk::PipelineRasterizationStateCreateInfo rasterization_create_info;
    rasterization_create_info.setRasterizerDiscardEnable(false)
                             .setCullMode(vk::CullModeFlagBits::eBack)
                             .setFrontFace(vk::FrontFace::eClockwise)
                             .setPolygonMode(vk::PolygonMode::eFill)
                             .setLineWidth(1.0f);
    pipeline_create_info.setPRasterizationState(&rasterization_create_info);

    // 6. multisampling
    vk::PipelineMultisampleStateCreateInfo multisampling_create_info;
    multisampling_create_info.setSampleShadingEnable(false)
                             .setRasterizationSamples(vk::SampleCountFlagBits::e1);
    pipeline_create_info.setPMultisampleState(&multisampling_create_info);

    // 7. test - stencil test, depth test

    // 8. color blending
    vk::PipelineColorBlendStateCreateInfo color_blend_create_info;
    vk::PipelineColorBlendAttachmentState color_blend_attachment;
    color_blend_attachment.setBlendEnable(false)
                          .setColorWriteMask(vk::ColorComponentFlagBits::eR | 
                                             vk::ColorComponentFlagBits::eG |
                                             vk::ColorComponentFlagBits::eB |
                                             vk::ColorComponentFlagBits::eA);
    
    color_blend_create_info.setLogicOpEnable(false)
                           .setAttachments(color_blend_attachment);

    pipeline_create_info.setPColorBlendState(&color_blend_create_info);

    // 9. renderpass and layout
    pipeline_create_info.setRenderPass(render_pass)
                        .setLayout(layout);

    auto res = device_.createGraphicsPipeline(nullptr, pipeline_create_info);
    if (res.result != vk::Result::eSuccess){
        throw std::runtime_error("failed to create graphics pipeline!");
    }
    pipeline = res.value;
    
}

void Render::createPipelineLayout()
{
    vk::PipelineLayoutCreateInfo pipeline_layout_create_info;

    layout = device_.createPipelineLayout(pipeline_layout_create_info);
}

void Render::createRenderPass()
{
    vk::RenderPassCreateInfo render_pass_create_info;
    vk::AttachmentDescription color_attachment;
    color_attachment.setFormat(swapchain_info.image_format.format)
                    .setInitialLayout(vk::ImageLayout::eUndefined)
                    .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
                    .setLoadOp(vk::AttachmentLoadOp::eClear)
                    .setStoreOp(vk::AttachmentStoreOp::eStore)
                    .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                    .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                    .setSamples(vk::SampleCountFlagBits::e1);
    render_pass_create_info.setAttachments(color_attachment);

    vk::SubpassDescription subpass;
    vk::AttachmentReference color_attachment_ref;
    color_attachment_ref.setAttachment(0)
                        .setLayout(vk::ImageLayout::eColorAttachmentOptimal);
    subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
           .setColorAttachments(color_attachment_ref);
    render_pass_create_info.setSubpasses(subpass);

    vk::SubpassDependency dependency; //多个subpass时，需要指定依赖关系,虽然只有一个subpass，但vulkan还有隐藏的subpass
    dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)
              .setDstSubpass(0)
              .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
              .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
              .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);

    render_pass_create_info.setDependencies(dependency);

    render_pass = device_.createRenderPass(render_pass_create_info);
}

void Render::createCommandPool()
{
    vk::CommandPoolCreateInfo command_pool_create_info;
    command_pool_create_info.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    command_pool = device_.createCommandPool(command_pool_create_info);

    vk::CommandBufferAllocateInfo command_buffer_allocate_info;
    command_buffer_allocate_info.setCommandPool(command_pool)
                                .setLevel(vk::CommandBufferLevel::ePrimary) //primary可以直接被gpu执行，secondary需要被primary调用
                                .setCommandBufferCount(1);
    command_buffer = device_.allocateCommandBuffers(command_buffer_allocate_info)[0];

}

void Render::createFence()
{
    vk::FenceCreateInfo info;
    cmd_avaliable_fence = device_.createFence(info);

    vk::SemaphoreCreateInfo semaphore_info;
    image_available_semaphore = device_.createSemaphore(semaphore_info);
    render_finished_semaphore = device_.createSemaphore(semaphore_info);
}

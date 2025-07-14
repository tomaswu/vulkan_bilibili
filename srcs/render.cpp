#include "render.hpp"
#include <SDL3/SDL_vulkan.h>
#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <SDL3/SDL.h>

Render::Render(SDL_Window *window)
{
    window_ = window;
    auto app_info = vk::ApplicationInfo();
    app_info.setApiVersion(VK_MAKE_VERSION(1, 3, 0));
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
}

Render::~Render()
{
    device_.destroySwapchainKHR(swapchain_);
    device_.destroy();
    instance_.destroySurfaceKHR(surface_);
    instance_.destroy();
}

void Render::render()
{

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

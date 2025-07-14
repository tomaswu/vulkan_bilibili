#include "render.hpp"
#include <SDL3/SDL_vulkan.h>
#include <iostream>

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
    vk::DeviceQueueCreateInfo device_queue_create_info;
    float queue_priority = 1.0f;
    device_queue_create_info.setPQueuePriorities(&queue_priority)
    .setQueueCount(1)
    .setQueueFamilyIndex(queue_family_indices.graphics_queue.value());

    device_cerate_info.setQueueCreateInfos(device_queue_create_info);

    device_ = physical_device_.createDevice(device_cerate_info);

    getQueues();
}

Render::~Render()
{
    device_.destroy();
    instance_.destroy();
}

void Render::render()
{

}

void Render::queryQueueFamilyIndices()
{
    auto queue_family_properties = physical_device_.getQueueFamilyProperties();
    for (int i=0;i<queue_family_properties.size();i++) 
    {
        auto &p = queue_family_properties[i];
        if(p.queueFlags & vk::QueueFlagBits::eGraphics){
            queue_family_indices.graphics_queue = i;
            break;
        }
    }
}

void Render::getQueues()
{
    graphics_queue = device_.getQueue(queue_family_indices.graphics_queue.value(), 0);
}

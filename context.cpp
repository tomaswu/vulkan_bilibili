#include "context.hpp"
#include <iostream>

namespace toy2d
{

    std::unique_ptr<Context> Context::instance_ = nullptr;

    void Context::Init(std::vector<const char*> &extensions,CreateSurfaceFunc func,int w,int h)
    {
        instance_.reset(new Context(extensions,func,w,h));
    }

    void Context::Quit()
    {
        instance_.reset();
    }

    Context &Context::GetInstance()
    {
        return *instance_;
    }

    Context::Context(std::vector<const char*> &extensions,CreateSurfaceFunc func,int w,int h)
    {
        createInstance(extensions);
        pickPhysicalDevice();
        queryQueueFamilyIndices(); 
        surface = func(instance);
        createDevice();
        getQueue();
        swapchain.reset(new Swapchain(w,h));
    }

    void Context::createInstance(std::vector<const char*> &extensions)
    {

        vk::InstanceCreateInfo createInfo;
        vk::ApplicationInfo appinfo;

        // layer 验证层等(非常重要)，API调用监控，有错就打出来
        // extension 扩展,比如说光追功能，RayTracingPipelineEXT

        /*  在mac上1.3.216以后，就需要先要将VK_INSTANCE_CREATE_ENUMRATE_PORTABILITY_BIT_KHR
            标志位添加到VkInstanceCreateInfo结构体的标志变量中去，
            然后将VK_KHR_PORTABILITY_ENUMRATE_EXTENSION_NAME添加到实例启用的扩展列表中去。
        */

        for (auto i = 0; i < extensions.size(); ++i)
        {
            std::cout << "extension name:" << extensions[i] << std::endl;
        }
        extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

        appinfo.setApiVersion(VK_API_VERSION_1_3);
        createInfo.setPApplicationInfo(&appinfo);
        createInfo.setFlags(vk::InstanceCreateFlags((VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR)));
        createInfo.setEnabledExtensionCount(extensions.size());
        createInfo.setPpEnabledExtensionNames(extensions.data());

        // !get all layer name
        /*  VK_LAYER_LUNARG_api_dump
            VK_LAYER_KHRONOS_profiles
            VK_LAYER_KHRONOS_validation
            VK_LAYER_LUNARG_screenshot
            VK_LAYER_KHRONOS_synchronization2
            VK_LAYER_KHRONOS_shader_object
        */
        std::cout << "-----------show all supported layers---------" << std::endl;
        auto avaiLayers = vk::enumerateInstanceLayerProperties();
        for (auto &layer : avaiLayers)
        {
            std::cout << layer.layerName << std::endl;
        }
        std::cout << "---------------------------------------------" << std::endl;

        std::vector<const char *> layers{"VK_LAYER_KHRONOS_validation"};
        createInfo.setPEnabledLayerNames(layers);

        instance = vk::createInstance(createInfo);
    }

    void Context::pickPhysicalDevice()
    {
        auto physical_device_list = instance.enumeratePhysicalDevices();
        std::cout << "find physical device count:" << physical_device_list.size() << std::endl;
        for (auto &device : physical_device_list)
        {
            std::cout << "physical device name:" << device.getProperties().deviceName << std::endl;
        }
        auto device = physical_device_list[0];
        auto fea = device.getFeatures();
        // 打印显卡类型

        std::cout << "physical device type:" << physicalDeviceType(device.getProperties().deviceType) << std::endl;
        std::cout << "support gemotry shader?" << fea.geometryShader << std::endl;
        std::cout << "vendor id:" << device.getProperties().vendorID << std::endl;
        std::cout << "api version:" << device.getProperties().apiVersion << std::endl;
        physical_device = device;
    }

    std::string Context::physicalDeviceType(vk::PhysicalDeviceType type)
    {
        switch (type)
        {
        case vk::PhysicalDeviceType::eCpu:
            return "eCpu";
            break;
        case vk::PhysicalDeviceType::eDiscreteGpu:
            return "eDiscreteGpu";
            break;

        case vk::PhysicalDeviceType::eIntegratedGpu:
            return "eIntegratedGpu";
            break;

        case vk::PhysicalDeviceType::eVirtualGpu:
            return "eVirtualGpu";
            break;

        case vk::PhysicalDeviceType::eOther:
            return "eOther";
            break;

        default:
            break;
        }
    }

    void Context::createDevice()
    {
        vk::DeviceCreateInfo createInfo;
        std::cout << "--------------device extension properties-------------" << std::endl;

        auto layers = physical_device.enumerateDeviceExtensionProperties();
        for (auto &l : layers)
        {
            std::cout << l.extensionName << std::endl;
        }
        std::cout << "-------------------------------------------------------" << std::endl;

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        float priorities = 1.0;
        if(queueFamilyIndices.presentQueue.value()==queueFamilyIndices.graphicsQueue.value()){
            vk::DeviceQueueCreateInfo queueCreateInfo;
            queueCreateInfo.setPQueuePriorities(&priorities);
            queueCreateInfo.setQueueCount(2);
            queueCreateInfo.setQueueFamilyIndex(queueFamilyIndices.graphicsQueue.value());
            queueCreateInfos.push_back(std::move(queueCreateInfo));
        }
        else{
            vk::DeviceQueueCreateInfo queueCreateInfo;
            queueCreateInfo.setPQueuePriorities(&priorities);
            queueCreateInfo.setQueueCount(1);
            queueCreateInfo.setQueueFamilyIndex(queueFamilyIndices.graphicsQueue.value());
            queueCreateInfos.push_back(queueCreateInfo);

            queueCreateInfo.setQueueFamilyIndex(queueFamilyIndices.presentQueue.value());

            queueCreateInfos.push_back(queueCreateInfo);

        }

        createInfo.setQueueCreateInfos(queueCreateInfos);
   
        createInfo.setEnabledExtensionCount(1);
        std::vector<const char*> extensionNames ={"VK_KHR_portability_subset"};
        createInfo.setPEnabledExtensionNames(extensionNames);
        device = physical_device.createDevice(createInfo);
    }

    void Context::queryQueueFamilyIndices()
    {
        auto properties  = physical_device.getQueueFamilyProperties();
        for(int i=0;i<properties.size();i++){
            auto support = properties[i].queueFlags | vk::QueueFlagBits::eGraphics;
            std::cout << "queue properties:"<<i<<"::"<< bool(support) << std::endl;
            if(support){
                queueFamilyIndices.graphicsQueue=i;
            }
            if(physical_device.getSurfaceSupportKHR(i,surface)){
                queueFamilyIndices.presentQueue = i;
            }
            if(queueFamilyIndices){
                break;
            }
        }

    }

    void Context::getQueue()
    {
       graphicsQueue = device.getQueue(queueFamilyIndices.graphicsQueue.value(),0);
       presentQueue = device.getQueue(queueFamilyIndices.presentQueue.value(),0); 
    }

    Context::~Context()
    {
        //! 特别注意，vulkan的销毁是有顺序的,和创建顺序相反
        swapchain.reset();
        device.destroy();
        instance.destroy();
    }

}
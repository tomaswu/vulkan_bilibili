#pragma once
#include <vulkan/vulkan.hpp>
#include <memory>
#include <string>
#include <optional>
#include "tool.hpp"
#include "swapchain.hpp"

namespace toy2d
{
    class Context final
    {

    public:
        static void Init(std::vector<const char*> &extensions, CreateSurfaceFunc func,int w,int h);
        static void Quit();
        static Context &GetInstance();

        ~Context();

        vk::Instance instance;
        vk::PhysicalDevice physical_device;
        vk::Device device;
        vk::SurfaceKHR surface;
        struct QueueFamilyIndices final{
            std::optional<uint32_t> graphicsQueue;
            std::optional<uint32_t> presentQueue;

            operator bool() const{
                return graphicsQueue.has_value() && presentQueue.has_value();
            }

        };

        QueueFamilyIndices queueFamilyIndices;
        vk::Queue graphicsQueue;
        vk::Queue presentQueue;
        std::unique_ptr<Swapchain> swapchain;

    private:
        Context(std::vector<const char*> &extensions,CreateSurfaceFunc func,int w,int h);
        static std::unique_ptr<Context> instance_;

        void createInstance(std::vector<const char*> &extensions);
        void pickPhysicalDevice();
        std::string physicalDeviceType(vk::PhysicalDeviceType type);

        void createDevice();
        void queryQueueFamilyIndices();
        void getQueue();
    };
} // namespace toy2d
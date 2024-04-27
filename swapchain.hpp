#pragma once

#include <vulkan/vulkan.hpp>

namespace toy2d
{
    class Swapchain final
    {
    public:
        vk::SwapchainKHR swapchain;

        Swapchain(int w,int h);
        ~Swapchain();

        struct SwapchainInfo
        {
            vk::Extent2D imageExtent;
            uint32_t imageCount;
            vk::SurfaceFormatKHR format;
            vk::SurfaceTransformFlagsKHR transform;
            vk::PresentModeKHR present_mode;
        };

        SwapchainInfo info;
        void queryInfo(int w,int h);

    private:
    };
}
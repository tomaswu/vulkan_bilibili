#include "swapchain.hpp"
#include "context.hpp"

namespace toy2d
{
    Swapchain::Swapchain(int w, int h)
    {
        queryInfo(w, h);
        vk::SwapchainCreateInfoKHR createinfo;
        createinfo.setClipped(true); // 如果cpu上的图比屏幕大，就进行裁剪
        // 在vulkan底层，所有的图片都是按数组来存储的，如果只有1个图，也放在数组中，所以这里设置为1，如果使用3DImage技术，就可以设置多层
        createinfo.setImageArrayLayers(1);
        createinfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);   // 颜色附件，后面来解释，允许gpu往屏幕上绘制彩色点
        createinfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque); // 颜色混合设置为不溶混
        createinfo.setSurface(Context::GetInstance().surface);
        createinfo.setImageColorSpace(info.format.colorSpace);
        createinfo.setImageFormat(info.format.format);
        createinfo.setImageExtent(info.imageExtent);
        createinfo.setMinImageCount(info.imageCount);
        createinfo.setPresentMode(info.present_mode);
        
        auto &queueIndices = Context::GetInstance().queueFamilyIndices;
        if(queueIndices.graphicsQueue.value()==queueIndices.presentQueue.value()){
             createinfo.setQueueFamilyIndices(queueIndices.graphicsQueue.value());
             //如果一个图像被多个命令队列使用，就需要指定一下使用方式。这里指定只能被一个队列使用。
             createinfo.setImageSharingMode(vk::SharingMode::eExclusive);
        }else{
            std::array indices = {queueIndices.graphicsQueue.value(),queueIndices.presentQueue.value()};
            createinfo.setQueueFamilyIndices(indices);
            // concurrent表示并行
            createinfo.setImageSharingMode(vk::SharingMode::eConcurrent);
        }

        swapchain = Context::GetInstance().device.createSwapchainKHR(createinfo);
    }

    void Swapchain::queryInfo(int w, int h)
    {
        auto &phyDevice = Context::GetInstance().physical_device;
        auto &surface = Context::GetInstance().surface;
        auto formats = phyDevice.getSurfaceFormatsKHR(surface);
        info.format = formats[0]; // 给它一个默认值
        for (const auto &format : formats)
        {
            if (format.format == vk::Format::eR8G8B8A8Srgb &&
                format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                info.format = format;
                break;
            }
        }
        auto capabilities = phyDevice.getSurfaceCapabilitiesKHR(surface);
        // clamp就是将设置值夹在最小和最大值之间
        info.imageCount = std::clamp<uint32_t>(2,capabilities.minImageCount, capabilities.maxImageCount);
        info.imageExtent.width = std::clamp<uint32_t>(w,capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        info.imageExtent.height = std::clamp<uint32_t>(h,capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        info.transform = capabilities.currentTransform;
        info.present_mode = vk::PresentModeKHR::eFifo;
        auto presents = phyDevice.getSurfacePresentModesKHR(surface);

        /*
            绘制第一个时，但想绘制第二个时，present mode就起作用了
            fifo表示先进先出显示图片，维护一个队列，一定被支持
            fifo relax 更加放宽的一种fifo，如果前面的没绘制完，要绘制后面的就会直接绘制，前面的会放弃，可能会产生撕裂现象，如果同时来两张的话，这两个还是会排队
            immediate 只要来了一个图像，就立即绘制，如果前面没绘制完也会放弃，一定会产生撕裂，但性能最高
            mailbox 假设屏幕上有个信箱，每次从里面取一个，里面也只有一个，向屏幕发送时，覆盖掉没有绘制的图像，也即只绘制最新的图像，一般游戏中都是这种
        */

        for (auto &p:presents){
            if(p==vk::PresentModeKHR::eMailbox){
               info.present_mode = p; 
            }
        }

    }

    Swapchain::~Swapchain()
    {
        Context::GetInstance().device.destroySwapchainKHR(swapchain);
    }

} // namespace toy2d

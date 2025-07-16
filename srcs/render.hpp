#include <vulkan/vulkan.hpp>
#include <QWindow>
#include <optional>
#include "vertex.hpp"
#include <QVulkanInstance>

class Render{
    public:
        Render(QWindow* window);
        ~Render();

        void render();

    private:
        //test
            float delata = 0.0001;
            float y = 0.5;
        // end test

        QWindow* window_{nullptr};
        QVulkanInstance *qinstance_{nullptr};
        vk::Instance instance_{nullptr};
        vk::SurfaceKHR surface_{nullptr};
        vk::PhysicalDevice physical_device_{nullptr};
        vk::Device device_{nullptr};

        struct QueueFamilyIndices final{
            std::optional<uint32_t> graphics_queue;
            std::optional<uint32_t> present_queue;
            operator bool() const{
                return graphics_queue.has_value() && present_queue.has_value();
            }
        };

        QueueFamilyIndices queue_family_indices;
        vk::Queue graphics_queue;
        vk::Queue present_queue;

        vk::SwapchainKHR swapchain_{nullptr};

        struct SwapchainInfo final{
            vk::Extent2D extent;
            uint32_t image_count;
            vk::SurfaceFormatKHR image_format;
            vk::SurfaceTransformFlagsKHR transform;
            vk::PresentModeKHR present_mode;
        };

        SwapchainInfo swapchain_info;

        std::vector<vk::ImageView> image_views;
        std::vector<vk::Framebuffer> framebuffers;

        vk::ShaderModule vert_shader_module;
        vk::ShaderModule frag_shader_module;

        vk::Pipeline pipeline;
        vk::PipelineLayout layout;
        vk::RenderPass render_pass;
        std::vector<vk::PipelineShaderStageCreateInfo> shader_stages;

        vk::CommandPool command_pool;
        vk::CommandBuffer command_buffer;
        vk::Fence cmd_avaliable_fence;
        vk::Semaphore image_available_semaphore;
        std::vector<vk::Semaphore> render_finished_semaphores;

        vk::Buffer vertex_buffer;
        vk::DeviceMemory vertex_buffer_memory;
        size_t vertex_buffer_size{24};

        void querySwapchainInfo(int width, int height);

        void queryQueueFamilyIndices();
        void getQueues();
        void createImageViews();
        void createFramebuffers();

        void createShaderModules();
        void createPipeline();
        void createPipelineLayout();
        void createRenderPass();

        void createCommandPool();
        void createFence();
        void createVertexBuffer();

        // assistant functions
        std::tuple<uint32_t, uint32_t> getWindowSize();
};
#include <vulkan/vulkan.hpp>
#include <SDL3/SDL.h>
#include <optional>

class Render{
    public:
        Render(SDL_Window* window);
        ~Render();

        void render();

    private:
        SDL_Window* window_{nullptr};
        vk::Instance instance_{nullptr};
        vk::SurfaceKHR surface_{nullptr};
        vk::PhysicalDevice physical_device_{nullptr};
        vk::Device device_{nullptr};

        struct QueueFamilyIndices final{
            std::optional<uint32_t> graphics_queue;
        };

        QueueFamilyIndices queue_family_indices;
        vk::Queue graphics_queue;

        void queryQueueFamilyIndices();
        void getQueues();
};
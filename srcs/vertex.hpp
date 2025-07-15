#include <vulkan/vulkan.hpp>

struct Vertex final{
    float x, y;

    static vk::VertexInputAttributeDescription getAttribute();
    static vk::VertexInputBindingDescription getBinding();
};
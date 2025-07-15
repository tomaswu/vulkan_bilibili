#include "vertex.hpp"

vk::VertexInputAttributeDescription Vertex::getAttribute()
{
    vk::VertexInputAttributeDescription attr;
    attr.setBinding(0)
        .setLocation(0)
        .setFormat(vk::Format::eR32G32Sfloat)
        .setOffset(0);
    return attr;
}

vk::VertexInputBindingDescription Vertex::getBinding()
{
    vk::VertexInputBindingDescription binding;
    binding.setBinding(0)
        .setInputRate(vk::VertexInputRate::eVertex)
        .setStride(sizeof(Vertex));
    return binding;
}
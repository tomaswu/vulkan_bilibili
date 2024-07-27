#pragma once

#include <vulkan/vulkan.hpp>
#include "context.hpp"
#include <functional>
#include "shader.hpp"
namespace toy2d
{


    void Init(std::vector<const char*> &extensions,CreateSurfaceFunc func,int w,int h);
    void Quit();
    
} // namespace toy2d
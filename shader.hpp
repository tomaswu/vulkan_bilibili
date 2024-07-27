#pragma once

#include "vulkan/vulkan.hpp"
#include <memory>
namespace toy2d
{
    class Shader final
    {
    public:
        static void Init(const std::string& vertex_source, const std::string& fragment_source);
        static void Quit();
        static Shader &GetInstance();

        vk::ShaderModule vertex_module;
        vk::ShaderModule fragment_module;

    private:
        Shader(const std::string& vertex_source, const std::string& fragment_source);
        ~Shader();

        static std::unique_ptr<Shader> instance_;
    };
}
#include "shader.hpp"
#include "context.hpp"

namespace toy2d
{
    std::unique_ptr<Shader> Shader::instance_ = nullptr;

    void Shader::Init(const std::string &vertex_source, const std::string &fragment_source)
    {
        instance_.reset(new Shader(vertex_source, fragment_source));
    }

    void Shader::Quit()
    {
    }
    Shader &Shader::GetInstance()
    {
        return *instance_; 
    }
    Shader::Shader(const std::string &vertex_source, const std::string &fragment_source)
    {
        vk::ShaderModuleCreateInfo cinfo;
        cinfo.setCodeSize(vertex_source.size());
        cinfo.pCode = reinterpret_cast<uint32_t *>(const_cast<char *>(vertex_source.data()));
        vertex_module = Context::GetInstance().device.createShaderModule(cinfo);

        vk::ShaderModuleCreateInfo cinfo1;
        cinfo1.setCodeSize(fragment_source.size());
        cinfo1.pCode = reinterpret_cast<uint32_t *>(const_cast<char *>(fragment_source.data()));
        fragment_module = Context::GetInstance().device.createShaderModule(cinfo1);
    }

    Shader::~Shader()
    {
        auto& device = Context::GetInstance().device;
        device.destroyShaderModule(vertex_module);
        device.destroyShaderModule(fragment_module);
    }
}
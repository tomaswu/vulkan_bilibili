#include "toy2d.hpp"


namespace toy2d
{
    void Init(std::vector<const char*> &extensions, CreateSurfaceFunc func,int w,int h)
    {
        Context::Init(extensions,func);
        Context::GetInstance().InitSwapchain(w,h);
        Shader::Init(ReadModuleFile("./vert.spv"),ReadModuleFile("./frag.spv"));
    }

    void Quit()
    {
        Shader::Quit();
        Context::GetInstance().DestroySwapchain();
        Context::Quit();
    }

} // namespace toy2d

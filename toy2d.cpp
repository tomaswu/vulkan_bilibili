#include "toy2d.hpp"


namespace toy2d
{
    void Init(std::vector<const char*> &extensions, CreateSurfaceFunc func,int w,int h)
    {
        Context::Init(extensions,func,w,h);
    }

    void Quit()
    {
        Context::Quit();
    }

} // namespace toy2d

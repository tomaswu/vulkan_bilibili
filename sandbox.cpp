#include <iostream>
#include <vulkan/vulkan.hpp>
#include "SDL.h"
#include "SDL_vulkan.h"
#include <vector>
#include "toy2d.hpp"

#undef main
int main()
{
    std::cout << "hello vulkan demo!!!" << std::endl;

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow("sandbox",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          1024, 720,
                                          SDL_WINDOW_VULKAN);

    if (!window)
    {
        SDL_Log("create window failed");
        exit(2);
    }

    bool shouldClose{false};
    SDL_Event event;

    uint32_t count;
    SDL_Vulkan_GetInstanceExtensions(window, &count, nullptr);
    std::vector<const char *> extensions(count);
    SDL_Vulkan_GetInstanceExtensions(window, &count, extensions.data());

    toy2d::Init(
        extensions, [&](vk::Instance instance)
        {
        VkSurfaceKHR surface;
        if(!SDL_Vulkan_CreateSurface(window,instance,&surface)){
            throw std::runtime_error("create surface failed!");
        }
        return surface; },
        1024,
        720);

    while (!shouldClose)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                shouldClose = true;
            }
        }
    }

    toy2d::Quit();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
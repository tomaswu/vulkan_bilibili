#include <iostream>
#include <SDL3/SDL.h>
#include "render.hpp"

int main() {
    std::cout << "Hello, world!" << std::endl;
    if (!SDL_Init(SDL_INIT_VIDEO)){
        SDL_Log("sdl init error:%s",SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window* win = SDL_CreateWindow("SDL3 minimal window", 640, 480, SDL_WINDOW_RESIZABLE|SDL_WINDOW_VULKAN);

    if(!win){
        return EXIT_FAILURE;
    }

    Render render(win);

    bool running{true};
    while(running){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                running = false;
                break;

            case SDL_EVENT_KEY_DOWN:
                if (event.key.key==SDLK_ESCAPE){
                    running = false;
                    break;
                }
            }
        }
    }
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}

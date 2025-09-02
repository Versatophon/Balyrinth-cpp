#pragma once

struct SDL_Window;
struct SDL_Renderer;
//struct SDL_GPUDevice;

class SdlResourcesProvider
{
public:
    virtual SDL_Window* GetSdlWindow() = 0;
};


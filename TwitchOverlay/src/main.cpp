
#if 1//Used to easy switch between integrated/discrete graphics
#ifdef WIN32
#include <cstdint>
#include <Windows.h>
// Use discrete GPU by default.
#ifdef __cplusplus
extern "C" {
#endif
    //http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
    //http://developer.amd.com/community/blog/2015/10/02/amd-enduro-system-for-developers/
    __declspec(dllexport) int32_t AmdPowerXpressRequestHighPerformance = 1;
#ifdef __cplusplus
}
#endif
#endif
#endif

#define SDL_MAIN_USE_CALLBACKS 1/* use the callbacks instead of main() */
extern "C" {
#include <SDL3/SDL_main.h>
}

#include "SdlApp.h"

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** pAppState, int pArgC, char** pArgV)
{
    SdlApp* lSdlApp = new SdlApp();
    *pAppState = lSdlApp;
    return (SDL_AppResult)lSdlApp->Init();
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* pAppState, SDL_Event* pEvent)
{
    return (SDL_AppResult)((SdlApp*)pAppState)->Event(pEvent);
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* pAppState)
{
    return (SDL_AppResult)((SdlApp*)pAppState)->Iterate();
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* pAppState, SDL_AppResult pResult)
{
    SdlApp* lSdlApp = ((SdlApp*)pAppState);
    lSdlApp->Quit();

    delete lSdlApp;
}

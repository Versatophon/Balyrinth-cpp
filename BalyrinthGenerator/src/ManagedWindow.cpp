#include "ManagedWindow.h"

#ifdef WIN32
#include <Windows.h>
#endif

#include <GL/gl.h>

#define SDL_MAIN_USE_CALLBACKS 1/* use the callbacks instead of main() */
#define SDL_MAIN_HANDLED 1/* as we define our own main entry point, we need to define this macro */

extern "C" {
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
}

#include <imgui.h>

#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

#include <cstdlib>

#include <iostream>

#include "Tools/SettingsLoadSave.h"

#include <Vector2f.h>
#include <Rectanglei.h>

SDL_AppResult AppInit(void **pAppstate, int32_t pArgC, char** pArgV);
SDL_AppResult AppEvent(void *pAppstate, SDL_Event *pEvent);
SDL_AppResult AppIterate(void *pAppState);
void AppQuit(void *pAppstate, SDL_AppResult pResult);

struct WindowGeometrySettings
{
    Rectanglei Rect = { {SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED}, {800, 600} };
    bool Maximized = false;

    void Save(const char* pFilepath)
    {
        SaveSetting(this, sizeof(*this), pFilepath);
    }

    void Load(const char* pFilepath)
    {
        LoadSetting(this, sizeof(*this), pFilepath);
    }
};

struct ManagedWindowID
{
    ManagedWindow* Window = nullptr;
    SDL_Window* SdlWindow = nullptr;
    SDL_GLContext SdlGlContext = nullptr;

    uint64_t LastFrameRawTime = SDL_MAX_UINT64;
    float LastFrameDuration = 0.f;

    WindowGeometrySettings GeometrySettings;

    Vector2i ActualSize = { 10, 10 };

    ManagedWindowID(ManagedWindow* pWindow):
    Window(pWindow)
    {
    }

    int32_t Init()
    {
        return Window->InternalInit();
    }

    int32_t Event(SDL_Event *pEvent)
    {
        switch (pEvent->type)
        {
        case SDL_EVENT_WINDOW_MOVED:
            if (!GeometrySettings.Maximized)
            {//Store this information only if app is not maximized
                GeometrySettings.Rect.Position.X = pEvent->display.data1;
                GeometrySettings.Rect.Position.Y = pEvent->display.data2;
            }
            break;

        case SDL_EVENT_WINDOW_RESIZED:
            ActualSize.Width = pEvent->display.data1;
            ActualSize.Height = pEvent->display.data2;
            if (!GeometrySettings.Maximized)
            {//Store this information only if app is not maximized
                GeometrySettings.Rect.Size = ActualSize;
            }

            Window->Resize(ActualSize);
            break;

        case SDL_EVENT_WINDOW_MAXIMIZED:
            GeometrySettings.Maximized = true;
            break;

        case SDL_EVENT_WINDOW_RESTORED:
            GeometrySettings.Maximized = false;
            break;
        }
                        

        return Window->InternalEvent(pEvent);
    }

    int32_t Iterate()
    {
        static const double lCounterFrequency = double(SDL_GetPerformanceFrequency());
        uint64_t lCurrentRawTime = SDL_GetPerformanceCounter();

        if( LastFrameRawTime < lCurrentRawTime)
        {
            LastFrameDuration = (double(lCurrentRawTime - LastFrameRawTime) / double(lCounterFrequency));
        }
        else
        {//Consider null duration here
            LastFrameDuration = 0.f;
        }
        LastFrameRawTime = lCurrentRawTime;

        return Window->InternalIterate();
    }

    void Quit()
    {
        Window->InternalQuit();
        GeometrySettings.Save("WinGeometry.settings");
    }
};


ManagedWindow::ManagedWindow(int32_t pArgC, char** pArgV):
mArgC(pArgC),
mID(new ManagedWindowID(this))
{
    if( mArgC > 0)
    {
        mArgV = new char*[mArgC];
        for (int32_t i = 0 ; i < mArgC ; ++i)
        {
            int32_t lStringLength = strlen(pArgV[i])+1;//copy with trailing '\0'
            mArgV[i] = new char[lStringLength];
            memcpy(mArgV[i], pArgV[i], lStringLength);
        }
    }
}

ManagedWindow::~ManagedWindow()
{
    for (int32_t i = 0 ; i < mArgC ; ++i)
    {
        delete mArgV[i];
    }
    delete mArgV;

    delete mID;
}

SDL_Window* ManagedWindow::GetSdlWindow()
{
    return mID->SdlWindow;
}

int32_t ManagedWindow::Execute()
{
    char* lArgV [1] = {(char*)this->mID};
    return SDL_EnterAppMainCallbacks(1, lArgV, AppInit, AppIterate, AppEvent, AppQuit);
}

int32_t ManagedWindow::Init()
{
    return SDL_APP_CONTINUE;
}

int32_t ManagedWindow::Event(SDL_Event *pEvent)
{
    return SDL_APP_CONTINUE;
}

int32_t ManagedWindow::Iterate()
{
    return SDL_APP_CONTINUE;
}

void ManagedWindow::Quit()
{
}

void ManagedWindow::Render()
{
}

void ManagedWindow::Resize(const Vector2i pSize)
{
}

float ManagedWindow::GetLastFrameDuration()
{
    return mID->LastFrameDuration;
}

uint32_t ManagedWindow::GetWidth()
{
    return mID->ActualSize.Width;
}

uint32_t ManagedWindow::GetHeight()
{
    return mID->ActualSize.Height;
}

int32_t ManagedWindow::InternalInit()
{
    SDL_SetAppMetadata("Balyrinth Generator", "0.0.1", "com.versatophon.balyrinth");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Enable native IME.
    //SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    //Use OpenGL 3.1 core
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    mID->GeometrySettings.Load("WinGeometry.settings");

    SDL_PropertiesID lWindowProperties = SDL_CreateProperties();
    SDL_SetStringProperty(lWindowProperties, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "Balyrinth Generator");
    SDL_SetNumberProperty(lWindowProperties, SDL_PROP_WINDOW_CREATE_X_NUMBER, mID->GeometrySettings.Rect.Position.X);
    SDL_SetNumberProperty(lWindowProperties, SDL_PROP_WINDOW_CREATE_Y_NUMBER, mID->GeometrySettings.Rect.Position.Y);
    SDL_SetNumberProperty(lWindowProperties, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, mID->GeometrySettings.Rect.Size.Width);
    SDL_SetNumberProperty(lWindowProperties, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, mID->GeometrySettings.Rect.Size.Height);
    SDL_SetBooleanProperty(lWindowProperties, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);
    SDL_SetBooleanProperty(lWindowProperties, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
    SDL_SetBooleanProperty(lWindowProperties, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, true);
    SDL_SetBooleanProperty(lWindowProperties, SDL_PROP_WINDOW_CREATE_MAXIMIZED_BOOLEAN, mID->GeometrySettings.Maximized);

    mID->ActualSize = mID->GeometrySettings.Rect.Size;

    mID->SdlWindow = SDL_CreateWindowWithProperties(lWindowProperties);

    SDL_DestroyProperties(lWindowProperties);

    if (mID->SdlWindow == nullptr)
    {
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    mID->SdlGlContext = SDL_GL_CreateContext(mID->SdlWindow);
    SDL_GL_MakeCurrent(mID->SdlWindow, mID->SdlGlContext);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    SDL_Log("renderer: %s", (const char*)glGetString(GL_RENDERER));

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(mID->SdlWindow, mID->SdlGlContext);
    ImGui_ImplOpenGL3_Init("#version 130");

    return Init();
}

int32_t ManagedWindow::InternalEvent(SDL_Event *pEvent)
{
    if (pEvent->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }

    bool lEventProcessedByImGui = ImGui_ImplSDL3_ProcessEvent(pEvent);

    if(lEventProcessedByImGui)
    {
        ImGuiIO& io = ImGui::GetIO();
        switch (pEvent->type)
        {
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            if (io.WantCaptureKeyboard)
            {//If keyboard is used by ImGui, don't transmit event to application
                return SDL_APP_CONTINUE;
            }
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
        case SDL_EVENT_MOUSE_MOTION:
        case SDL_EVENT_MOUSE_WHEEL:
            if (io.WantCaptureMouse)
            {//If mouse is used by ImGui, don't transmit event to application
                return SDL_APP_CONTINUE;
            }
            break;
        }
    }

    return Event(pEvent);

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

int32_t ManagedWindow::InternalIterate()
{
    //Call render to update window content
    Render();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    int32_t lReturnValue = Iterate();

    //Render
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(mID->SdlWindow);

    return lReturnValue;
}

void ManagedWindow::InternalQuit()
{
    Quit();
}

/* This function runs once at startup. */
SDL_AppResult AppInit(void** pAppState, int pArgC, char** pArgV)
{
    *pAppState = pArgV[0];
    return (SDL_AppResult)((ManagedWindowID*)(*pAppState))->Init();
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult AppEvent(void* pAppState, SDL_Event *pEvent)
{
     return (SDL_AppResult)((ManagedWindowID*)pAppState)->Event(pEvent);
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult AppIterate(void *pAppState)
{
    return (SDL_AppResult)((ManagedWindowID*)pAppState)->Iterate();
}

/* This function runs once at shutdown. */
void AppQuit(void *pAppState, SDL_AppResult pResult)
{
    ManagedWindowID* lWindowId = ((ManagedWindowID*)pAppState);
    lWindowId->Quit();

    ImGui_ImplSDL3_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();

    SDL_DestroyWindow(lWindowId->SdlWindow);
    SDL_Quit();
}

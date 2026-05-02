
#ifndef __EMSCRIPTEN__
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

#endif

#define SDL_MAIN_USE_CALLBACKS 1/* use the callbacks instead of main() */

extern "C" {
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_time.h>
}

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>

#include <string>
#include <format>

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/gl.h>
#endif

struct DecimalValueListener
{
    uint8_t Value = 10;

    bool SetValue(uint8_t pNewValue)
    {
        if (Value != pNewValue)
        {
            Value = pNewValue;
            return true;
        }
        return false;
    }
};

class SDLApp
{
public:
    SDLApp()
    {

    }

    SDL_AppResult Init()
    {
        return SDL_APP_CONTINUE;
    }

    SDL_AppResult Event(SDL_Event* pEvent)
    {
        if (pEvent->type == SDL_EVENT_QUIT) {
            return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
        }

        bool lEventProcessedByImGui = ImGui_ImplSDL3_ProcessEvent(pEvent);

        if (lEventProcessedByImGui)
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

        return SDL_APP_CONTINUE;  /* carry on with the program! */
    }

    SDL_AppResult Iterate()
    {
        glClearColor(0.f, 0.f, 0.f, 0.f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();



#ifdef IMGUI_HAS_VIEWPORT
        ImGuiViewport* lViewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(lViewport->GetWorkPos());
        ImGui::SetNextWindowSize(lViewport->GetWorkSize());
        ImGui::SetNextWindowViewport(lViewport->ID);
#else 
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
#endif
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.f });
        ImGui::Begin("Main", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs);
        
        {
            SDL_Time lTicks;
            SDL_DateTime lLocalTime;
            //SDL_DateTime lUniversalTime;
            SDL_GetCurrentTime(&lTicks);
            SDL_TimeToDateTime(lTicks, &lLocalTime, true);
            //SDL_TimeToDateTime(lTicks, &lUniversalTime, false);

            std::string lTimeAsString = std::format("{:02d}:{:02d}:{:02d}", lLocalTime.hour, lLocalTime.minute, lLocalTime.second);

            ImVec2 lCursorPos = ImGui::GetCursorPos();
            ImVec2 lContentAvail = ImGui::GetContentRegionAvail();

            ImGui::GetWindowDrawList()->AddRectFilled(lCursorPos, { lCursorPos.x + lContentAvail.x, lCursorPos.y + lContentAvail.y }, ImGui::ColorConvertFloat4ToU32({0.f, .5f, .5f, .5f}), 5.f);

            ImGui::PushFont(mTimeFont, 70.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.f , 0.f });

            ImVec2 lTextDims = ImGui::CalcTextSize(lTimeAsString.c_str());
            ImVec2 lTextPosition = { ImGui::GetContentRegionAvail().x - lTextDims.x, ImGui::GetContentRegionAvail().y - lTextDims.y };
            ImVec2 lTextEndPosition = { lTextPosition.x + lTextDims.x, lTextPosition.y + lTextDims.y };

            ImGui::GetWindowDrawList()->AddRectFilled(lTextPosition, lTextEndPosition, ImGui::ColorConvertFloat4ToU32({.5f, .5f, 0.f, .5f}), 5.f);

            ImGui::SetCursorPos(lTextPosition);
            ImGui::Text(lTimeAsString.c_str());

            ImGui::PopStyleVar();
            ImGui::PopFont();
        }


        ImGui::End();
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();


        if (ImGui::Begin("Control"))
        {
            SDL_Time lTicks;
            SDL_DateTime lLocalTime;
            SDL_DateTime lUniversalTime;
            SDL_GetCurrentTime(&lTicks);
            SDL_TimeToDateTime(lTicks, &lLocalTime, true);
            SDL_TimeToDateTime(lTicks, &lUniversalTime, false);
            //lTicks.

            ImGui::Text("%02d:%02d:%02d", lLocalTime.hour, lLocalTime.minute, lLocalTime.second);
            ImGui::Text("%02d:%02d:%02d", lUniversalTime.hour, lUniversalTime.minute, lUniversalTime.second);

            if (mHoursTensDigit.SetValue(lLocalTime.hour / 10))
                mHoursTensDigitChanged = !mHoursTensDigitChanged;

            if (mHoursUnitsDigit.SetValue(lLocalTime.hour % 10))
                mHoursUnitsDigitChanged = !mHoursUnitsDigitChanged;

            if (mMinutesTensDigit.SetValue(lLocalTime.minute / 10))
                mMinutesTensDigitChanged = !mMinutesTensDigitChanged;

            if (mMinutesUnitsDigit.SetValue(lLocalTime.minute % 10))
                mMinutesUnitsDigitChanged = !mMinutesUnitsDigitChanged;

            if (mSecondsTensDigit.SetValue(lLocalTime.second / 10))
                mSecondsTensDigitChanged = !mSecondsTensDigitChanged;

            if (mSecondsUnitsDigit.SetValue(lLocalTime.second % 10))
                mSecondsUnitsDigitChanged = !mSecondsUnitsDigitChanged;

            ImGui::Checkbox("Hours Tens", &mHoursTensDigitChanged);
            ImGui::Checkbox("Hours Units", &mHoursUnitsDigitChanged);
            ImGui::Checkbox("Minutess Tens", &mMinutesTensDigitChanged);
            ImGui::Checkbox("Minutess Units", &mMinutesUnitsDigitChanged);
            ImGui::Checkbox("Seconds Tens", &mSecondsTensDigitChanged);
            ImGui::Checkbox("Seconds Units", &mSecondsUnitsDigitChanged);


        }
        ImGui::End();

        //Render
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(mSdlMainWindow);

        return SDL_APP_CONTINUE;
    }

    void Quit()
    {

    }

private:
    friend SDL_AppResult SDL_AppInit(void** pAppState, int pArgC, char** pArgV);
    friend SDL_AppResult SDL_AppEvent(void* pAppState, SDL_Event* pEvent);
    friend SDL_AppResult SDL_AppIterate(void* pAppState);
    friend void SDL_AppQuit(void* pAppState, SDL_AppResult pResult);

    SDL_Window* mSdlMainWindow = nullptr;
    SDL_GLContext mSdlGlContext = nullptr;

    DecimalValueListener mHoursTensDigit;
    DecimalValueListener mHoursUnitsDigit;

    DecimalValueListener mMinutesTensDigit;
    DecimalValueListener mMinutesUnitsDigit;

    DecimalValueListener mSecondsTensDigit;
    DecimalValueListener mSecondsUnitsDigit;

    bool mHoursTensDigitChanged = false;
    bool mHoursUnitsDigitChanged = false;
    bool mMinutesTensDigitChanged = false;
    bool mMinutesUnitsDigitChanged = false;
    bool mSecondsTensDigitChanged = false;
    bool mSecondsUnitsDigitChanged = false;

    ImFont* mTimeFont = nullptr;
};

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** pAppState, int pArgC, char** pArgV)
{
    SDLApp* lSDLApp = new SDLApp();
    *pAppState = lSDLApp;

    SDL_Log("InternalInit start");

    SDL_SetAppMetadata("Twitch Overlay", "0.0.1", "com.versatophon.twitch_overlay");

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

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
#ifdef __EMSCRIPTEN__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
    //Use OpenGL 3.1 core
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif
    //SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_PropertiesID lWindowProperties = SDL_CreateProperties();
    SDL_SetStringProperty(lWindowProperties, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "Twitch Overlay");
    SDL_SetNumberProperty(lWindowProperties, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_UNDEFINED);
    SDL_SetNumberProperty(lWindowProperties, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_UNDEFINED);
#ifdef __EMSCRIPTEN__
    SDL_SetNumberProperty(lWindowProperties, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 1920);
    SDL_SetNumberProperty(lWindowProperties, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 1080);
#else
    SDL_SetNumberProperty(lWindowProperties, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 1280);
    SDL_SetNumberProperty(lWindowProperties, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 720);
#endif
    SDL_SetBooleanProperty(lWindowProperties, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);
    SDL_SetBooleanProperty(lWindowProperties, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
    SDL_SetBooleanProperty(lWindowProperties, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, true);

    lSDLApp->mSdlMainWindow = SDL_CreateWindowWithProperties(lWindowProperties);

    SDL_DestroyProperties(lWindowProperties);

    if (lSDLApp->mSdlMainWindow == nullptr)
    {
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    lSDLApp->mSdlGlContext = SDL_GL_CreateContext(lSDLApp->mSdlMainWindow);
    SDL_GL_MakeCurrent(lSDLApp->mSdlMainWindow, lSDLApp->mSdlGlContext);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    //lSDLApp->mTimeFont = io.Fonts->AddFontFromFileTTF("segoeuib.ttf");

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(lSDLApp->mSdlMainWindow, lSDLApp->mSdlGlContext);

#ifdef __EMSCRIPTEN__
    ImGui_ImplOpenGL3_Init("#version 300 es");
#else
    ImGui_ImplOpenGL3_Init("#version 330");
#endif
    //ImGui_ImplOpenGL3_Init("#version 130");

    return lSDLApp->Init();
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* pAppState, SDL_Event* pEvent)
{
    return (SDL_AppResult)((SDLApp*)pAppState)->Event(pEvent);
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* pAppState)
{
    return (SDL_AppResult)((SDLApp*)pAppState)->Iterate();
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* pAppState, SDL_AppResult pResult)
{
    SDLApp* lSDLApp = ((SDLApp*)pAppState);
    lSDLApp->Quit();

    delete lSDLApp;
}

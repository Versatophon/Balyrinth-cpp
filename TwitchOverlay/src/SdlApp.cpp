#include "SdlApp.h"

#ifdef WIN32
#endif

extern "C" {
#include <SDL3/SDL.h>
//#include <SDL3/SDL_time.h>
}

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>

#include <format>

#include <iostream>
#include <fstream>
#include <filesystem>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <GLES3/gl3.h>

extern "C" {
    EMSCRIPTEN_KEEPALIVE
        void OnSyncFsSuccess()
    {
        std::cout << "Sync FS OK" << std::endl;
        SdlApp::sFSReady = true;
    }

    EMSCRIPTEN_KEEPALIVE
        void OnSyncFsFailure()
    {
        std::cout << "Sync FS Not OK" << std::endl;
        SdlApp::sFSReady = true;
    }
}

EM_JS(void, on_syncfs_success, (), { Module._OnSyncFsSuccess(); });

EM_JS(void, on_syncfs_failure, (), { Module._OnSyncFsFailure(); });

#else
#include <Windows.h>
#include <GL/gl.h>
#endif

bool SdlApp::sFSReady = false;

SdlApp::SdlApp()
{
}

int32_t SdlApp::Init()
{
    SDL_SetAppMetadata("Twitch Overlay", "0.0.1", "com.versatophon.twitch_overlay");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

#ifdef __EMSCRIPTEN__
    EM_ASM(
        // Make a directory other than '/'
        // 
        FS.mkdir('/storage');
    // Then mount with IDBFS type
    FS.mount(IDBFS, {}, '/storage');

    FS.syncfs(true, function(err) {
        if (err)
        {
            on_syncfs_failure();
        }
        else
        {
            on_syncfs_success();
        }
    });
    console.log("storage ok")
        );

    mAppPath = "/storage/";
#else
    mAppPath = SDL_GetPrefPath("Versatophon", "TwitchOverlay");
#endif

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

    mSdlMainWindow = SDL_CreateWindowWithProperties(lWindowProperties);

    SDL_DestroyProperties(lWindowProperties);

    if (mSdlMainWindow == nullptr)
    {
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    mSdlGlContext = SDL_GL_CreateContext(mSdlMainWindow);
    SDL_GL_MakeCurrent(mSdlMainWindow, mSdlGlContext);
    SDL_GL_SetSwapInterval(1); // Enable vsync

#ifndef __EMSCRIPTEN__//Init directly when native app
    Initialize();
#endif

    return SDL_APP_CONTINUE;
}

int32_t SdlApp::Event(SDL_Event* pEvent)
{
    if (!mImGuiInitialized)
    {
        return SDL_APP_CONTINUE;
    }

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

#ifdef __EMSCRIPTEN__
            if (pEvent->key.key == SDLK_SPACE && pEvent->type == SDL_EVENT_KEY_UP)
            {
                EM_ASM(FS.syncfs(function(err) { if (err != null) console.log(err) }););
            }
#endif
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

int32_t SdlApp::Iterate()
{
    if (!mImGuiInitialized)
    {
        if (SdlApp::sFSReady)
        {
            Initialize();
        }
        else
        {
            return SDL_APP_CONTINUE;
        }
    }

    glClearColor(0.f, 0.f, 0.f, 0.f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();


    RenderBackgroundWindow();

    RenderControlWindow();

    RenderNumbersWindow();

    //Render
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(mSdlMainWindow);

    return SDL_APP_CONTINUE;
}

void SdlApp::Quit()
{
    delete mImGuiIniPath;
}

void SdlApp::Initialize()
{
    if (std::filesystem::is_directory(mAppPath))
    {
        std::filesystem::create_directory(mAppPath);

        std::cout << "dierctory created!" << std::endl;
    }

    std::string lImGuiIniPath = mAppPath + "imgui.ini";
    mImGuiIniPath = new char[lImGuiIniPath.size() + 1];
    memcpy(mImGuiIniPath, lImGuiIniPath.c_str(), lImGuiIniPath.size() + 1);


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    //io.IniFilename = nullptr;
    io.IniFilename = mImGuiIniPath;

    std::cout << "imgui initialized!" << std::endl;


    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    //lSDLApp->mTimeFont = io.Fonts->AddFontFromFileTTF("segoeuib.ttf");

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(mSdlMainWindow, mSdlGlContext);

#ifdef __EMSCRIPTEN__
    ImGui_ImplOpenGL3_Init("#version 300 es");
#else
    ImGui_ImplOpenGL3_Init("#version 330");
#endif
    //ImGui_ImplOpenGL3_Init("#version 130");

    mImGuiInitialized = true;
}

#include "NumberMatrices.h"
static int32_t sIndex = 0;
static uint32_t sReroll = 1000;

void SdlApp::RenderBackgroundWindow()
{
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

        ImGui::GetWindowDrawList()->AddRectFilled(lCursorPos, { lCursorPos.x + lContentAvail.x, lCursorPos.y + lContentAvail.y }, ImGui::ColorConvertFloat4ToU32({ 0.f, .5f, .5f, .5f }), 5.f);

        ImGui::PushFont(mTimeFont, 70.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.f , 0.f });

        ImVec2 lTextDims = ImGui::CalcTextSize(lTimeAsString.c_str());
        ImVec2 lTextPosition = { ImGui::GetContentRegionAvail().x - lTextDims.x, ImGui::GetContentRegionAvail().y - lTextDims.y };
        ImVec2 lTextEndPosition = { lTextPosition.x + lTextDims.x, lTextPosition.y + lTextDims.y };

        ImGui::GetWindowDrawList()->AddRectFilled(lTextPosition, lTextEndPosition, ImGui::ColorConvertFloat4ToU32({ .5f, .5f, 0.f, .5f }), 5.f);

        ImGui::SetCursorPos(lTextPosition);
        ImGui::Text("%s", lTimeAsString.c_str());

        ImGui::PopStyleVar();
        ImGui::PopFont();
    }
    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
}

void SdlApp::RenderControlWindow()
{
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

        ImGui::Text("App path: %s", mAppPath.c_str());
        ImGui::Text("file size: %i", int(mFileSize));
    }
    ImGui::End();
}

void SdlApp::RenderNumbersWindow()
{
    if (ImGui::Begin("Numbers"))
    {
        if (ImGui::SliderInt("Value", &sIndex, 0, 9))
        {
            sIndex = sIndex > 9 ? 9 : sIndex;
            sIndex = sIndex < 0 ? 0 : sIndex;
        }

        if (++sReroll >= 120)
        {
            //sIndex = rand() % 10;
            sReroll = 0;
        }

        const uint32_t lDigitCount = 10;
        const uint8_t* lFirstLine = sNumbersMatrix + sIndex*sizeof(uint32_t);
        const uint32_t lLineWidth = lDigitCount* sizeof(uint32_t);

        const uint32_t lDotSize = 10;

        ImVec2 lMin, lMax;

        for (uint32_t j = 0; j < 64; ++j)
        {
            const uint8_t* lLine = lFirstLine + (j * lLineWidth);

            lMin.y = ImGui::GetWindowPos().y + 60 + lDotSize * j;
            lMax.y = ImGui::GetWindowPos().y + 60 + lDotSize * (j + 1);

            for (uint32_t k = 0; k < sizeof(uint32_t); ++k)
            {
                uint8_t lQuarter = lLine[k];
                for (uint32_t i = 0; i < 8; ++i)
                {
                    if (lQuarter >> (7 - i) & 0b1)
                    {
                        lMin.x = ImGui::GetWindowPos().x + lDotSize * (k * 8 + i);
                        lMax.x = ImGui::GetWindowPos().x + lDotSize * (k * 8 + i + 1);
                        ImGui::GetWindowDrawList()->AddRectFilled(lMin, lMax, ImGui::GetColorU32({1.f, 0.f, 1.f, 1.f}));
                    }
                }
            }
        }
    }
    ImGui::End();
}

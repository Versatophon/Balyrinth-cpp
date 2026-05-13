#pragma once

#include <cstdint>
#include <string>

extern "C" {
#include <SDL3/SDL_init.h>
}

struct ImFont;
struct SDL_Window;
struct SDL_GLContextState;

struct DecimalValueListener
{
    uint8_t Value = 10;

    inline bool SetValue(uint8_t pNewValue)
    {
        if (Value != pNewValue)
        {
            Value = pNewValue;
            return true;
        }
        return false;
    }
};

//SdlApp
class SdlApp
{
public:
    SdlApp();

    SDL_AppResult Init();
    SDL_AppResult Event(SDL_Event* pEvent);
    SDL_AppResult Iterate();
    void Quit();

private:

    SDL_Window* mSdlMainWindow = nullptr;
    SDL_GLContextState* mSdlGlContext = nullptr;

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

    std::string mAppPath;

    size_t mFileSize = 0;

    char* mImGuiIniPath = nullptr;
    bool mImGuiInitialized = false;

    void Initialize();

public:
    static bool sFSReady;
};

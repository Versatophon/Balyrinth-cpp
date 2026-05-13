#pragma once

#include <cstdint>
#include <string>

struct ImFont;
union SDL_Event;
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

struct CoreData;


//SdlApp
class SdlApp
{
public:
    SdlApp();

    int32_t Init();
    int32_t Event(SDL_Event* pEvent);
    int32_t Iterate();
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

    void RenderBackgroundWindow();
    void RenderControlWindow();
    void RenderNumbersWindow();

    void GenerateTopologies();

    CoreData* mCoreData = nullptr;

public:
    static bool sFSReady;
};

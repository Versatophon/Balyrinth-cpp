#pragma once

#include <string>
#include <cstdint>

class GraphicsState
{
public:
    void Init();

    const char* GetRendererName() const;
    bool GetMemoryUsage(float& pMemoryUsageNormalized, const char** pMemoryUsageText) const;

private:
    int32_t mTotalMemory = 0;
    int32_t mCheckFreeMemoryEnum = 0;
    std::string mRendererName = "";
    mutable std::string mTextStateBuffer;
};

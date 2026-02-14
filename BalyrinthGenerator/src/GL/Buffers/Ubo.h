#pragma once

#include "Buffer.h"

#include <string>
#include <cstdint>

class Ubo :public Buffer
{
public:
    Ubo(uint32_t pSize, const char* pName);

    ~Ubo();

    void UpdateGpu();

    void* GetMemory();

    uint32_t GetBindingPoint();

    const char* GetBindingName();

private:
    uint32_t mBindingPoint;//Binding point

    std::string mBindingName;

    uint32_t mSize;
    void* mMemory = nullptr;

    static uint32_t sLastUboIndex;
};

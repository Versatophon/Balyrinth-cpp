#pragma once

#include "Buffer.h"
#include "../Enums.h"

#include <cstdint>

class ArrayBuffer :public Buffer
{
public:
    ArrayBuffer(uint32_t pSize, BufferUsage pBufferUsage, const void* pData = nullptr);
};

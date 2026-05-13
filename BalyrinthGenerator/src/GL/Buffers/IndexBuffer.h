#pragma once

#include "Buffer.h"

#include <cstdint>

#include "../Enums.h"

class IndexBuffer :public Buffer
{
public:
    IndexBuffer(uint32_t pSize, BufferUsage pBufferUsage, const void* pData = nullptr);
};


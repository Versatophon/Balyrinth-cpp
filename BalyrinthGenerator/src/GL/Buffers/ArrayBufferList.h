#pragma once

#include <cstdint>

enum class BufferUsage;
class ArrayBuffer;

class ArrayBufferList
{
public:
    ArrayBufferList(uint32_t pBufferCount, const BufferUsage* pBufferUsage);
    ~ArrayBufferList();

private:
    uint32_t mBufferCount = 0;
    ArrayBuffer* mBuffers = nullptr;
};

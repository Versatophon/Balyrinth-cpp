#include "ArrayBuffer.h"

#include <GL/glew.h>

ArrayBuffer::ArrayBuffer(uint32_t pSize, BufferUsage pBufferUsage, const void* pData) :
    Buffer(GL_ARRAY_BUFFER, pSize, GL_STREAM_DRAW + (4 * (uint32_t)pBufferUsage), pData)
{
}

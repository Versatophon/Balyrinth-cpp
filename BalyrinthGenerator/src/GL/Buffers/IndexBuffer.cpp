#include "IndexBuffer.h"

#include <GL/glew.h>

IndexBuffer::IndexBuffer(uint32_t pSize, BufferUsage pBufferUsage, const void* pData) :
    Buffer(GL_ELEMENT_ARRAY_BUFFER, pSize, GL_STREAM_DRAW + (4 * (uint32_t)pBufferUsage), pData)
{
}

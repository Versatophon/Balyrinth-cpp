#include "Ubo.h"

#include <GL/glew.h>

#include <cstring>

uint32_t Ubo::sLastUboIndex = 0;

void Ubo::UpdateGpu()
{
    Bind();
#if 1
    glBufferSubData(mTarget, 0, mSize, mMemory);
#else
    GLvoid* lDestMemory = glMapBuffer(mTarget, GL_WRITE_ONLY);//mappable ?
    if (lDestMemory != nullptr)
    {
        memcpy(lDestMemory, mMemory, mSize);
    }
    glUnmapBuffer(mTarget);
#endif
    Unbind();
}

uint32_t Ubo::GetBindingPoint()
{
    return mBindingPoint;
}

const char* Ubo::GetBindingName()
{
    return mBindingName.c_str();
}

Ubo::Ubo(uint32_t pSize, const char* pName) :
    Buffer(GL_UNIFORM_BUFFER, pSize, GL_DYNAMIC_DRAW, nullptr),
    mBindingName(pName),
    mSize(pSize),
    mMemory(new uint8_t[mSize])
{
    mBindingPoint = sLastUboIndex++;
    glBindBufferBase(mTarget, mBindingPoint, mId);
}

Ubo::~Ubo()
{
    delete (uint8_t*)mMemory;
}

void* Ubo::GetMemory()
{
    return mMemory;
}

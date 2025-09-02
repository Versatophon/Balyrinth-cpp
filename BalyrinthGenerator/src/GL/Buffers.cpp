#include "Buffers.h"

#include <cstring>
#include <iostream>

static uint32_t sLastUboIndex = 0;

static std::unordered_map<uint32_t, uint32_t> sTargetToBinding =
{
    {GL_ARRAY_BUFFER, GL_ARRAY_BUFFER_BINDING},
    {GL_ATOMIC_COUNTER_BUFFER, GL_ATOMIC_COUNTER_BUFFER_BINDING},
    {GL_COPY_READ_BUFFER, GL_COPY_READ_BUFFER_BINDING},
    {GL_COPY_WRITE_BUFFER, GL_COPY_WRITE_BUFFER_BINDING},
    {GL_DISPATCH_INDIRECT_BUFFER, GL_DISPATCH_INDIRECT_BUFFER_BINDING},
    {GL_DRAW_INDIRECT_BUFFER, GL_DRAW_INDIRECT_BUFFER_BINDING},
    {GL_ELEMENT_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER_BINDING},
    {GL_PIXEL_PACK_BUFFER, GL_PIXEL_PACK_BUFFER_BINDING},
    {GL_PIXEL_UNPACK_BUFFER, GL_PIXEL_UNPACK_BUFFER_BINDING},
    {GL_QUERY_BUFFER, GL_QUERY_BUFFER_BINDING},
    {GL_SHADER_STORAGE_BUFFER, GL_SHADER_STORAGE_BUFFER_BINDING},
    {GL_TEXTURE_BUFFER, GL_TEXTURE_BUFFER_BINDING},
    {GL_TRANSFORM_FEEDBACK_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER_BINDING},
    {GL_UNIFORM_BUFFER, GL_UNIFORM_BUFFER_BINDING},
};

Buffer::Buffer(uint32_t pTarget, size_t pSize, uint32_t pUsage, void* pData) :
    mTarget(pTarget),
    mSize(pSize),
    mUsage(pUsage),
    mTargetBinding(sTargetToBinding[mTarget])
{
    glGenBuffers(1, &mId);
    glBindBuffer(mTarget, mId);
    glBufferData(mTarget, mSize, pData, mUsage);
    glBindBuffer(mTarget, 0);
}

Buffer::~Buffer()
{
    glDeleteBuffers(1, &mId);
}

void Buffer::Bind() const
{
    glGetIntegerv(mTargetBinding, (int32_t*)&mPreviousId);
    glBindBuffer(mTarget, mId);
}

void Buffer::Debind() const
{
    glBindBuffer(mTarget, mPreviousId);
}

void Buffer::Upload(size_t pSize, void* pData)
{
    mSize = pSize;
    Bind();
    glBufferData(mTarget, mSize, pData, mUsage);
    Debind();
}

void Buffer::PartialUpload(size_t pOffset, size_t pSize, void* pData)
{
    Bind();
    glBufferSubData(mTarget, pOffset, pSize, pData);
    Debind();
}

ArrayBuffer::ArrayBuffer(uint32_t pSize, BufferUsage pBufferUsage, void* pData) :
    Buffer(GL_ARRAY_BUFFER, pSize, GL_STREAM_DRAW + (4 * (uint32_t)pBufferUsage), pData)
{
}

IndexBuffer::IndexBuffer(uint32_t pSize, BufferUsage pBufferUsage, void* pData) :
    Buffer(GL_ELEMENT_ARRAY_BUFFER, pSize, GL_STREAM_DRAW + (4 * (uint32_t)pBufferUsage))
{
}

void Ubo::UpdateGpu()
{
    Bind();
    GLvoid* lDestMemory = glMapBuffer(mTarget, GL_WRITE_ONLY);//mappable ?
    if (lDestMemory != nullptr)
    {
        memcpy(lDestMemory, mMemory, mSize);
    }
    glUnmapBuffer(mTarget);
    Debind();
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

void Vao::Init(ShaderProgram* pShaderProgram, ArrayBuffer** pArrayBuffers)
{
    if (mId == 0)
    {
        glGenVertexArrays(1, &mId);
        //Deinit();
    }


    Bind();

    uint32_t lAttributeCount = pShaderProgram->GetAttributeCount();
    for (uint32_t i = 0; i < lAttributeCount; ++i)
    {
        pArrayBuffers[i]->Bind();
        glEnableVertexAttribArray(pShaderProgram->GetAttribute(i));
        switch (pShaderProgram->GetAttributeType(i))
        {
        case AttributeType::FLOAT:
            glVertexAttribPointer(pShaderProgram->GetAttribute(i), 3, GL_FLOAT, false/*(i != 0)*/, 3 * sizeof(GLfloat), 0);
            break;
        case AttributeType::INTEGER:
            glVertexAttribIPointer(pShaderProgram->GetAttribute(i), 1, GL_UNSIGNED_BYTE, 1 * sizeof(GLubyte), 0);
            break;
        case AttributeType::DOUBLE:
            glVertexAttribIPointer(pShaderProgram->GetAttribute(i), 1, GL_DOUBLE, 1 * sizeof(GLdouble), 0);
            break;
        }
        pArrayBuffers[i]->Debind();
    }

    Debind();
}

void Vao::Deinit()
{
    glDeleteVertexArrays(1, &mId);
    mId = 0;
}

void Vao::Bind() const
{
    glBindVertexArray(mId);
}

void Vao::Debind() const
{
    glBindVertexArray(0);
}

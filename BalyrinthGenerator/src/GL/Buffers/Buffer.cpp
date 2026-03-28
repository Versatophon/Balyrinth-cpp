#include "Buffer.h"

#include <cstring>
#include <iostream>

#include <unordered_map>

#include <GL/glew.h>

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

Buffer::Buffer(uint32_t pTarget, size_t pSize, uint32_t pUsage, const void* pData) :
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

void Buffer::Unbind() const
{
    glBindBuffer(mTarget, mPreviousId);
}

void Buffer::SetSize(size_t pSize)
{
    mSize = pSize;
    Bind();
    glBufferData(mTarget, mSize, nullptr, mUsage);
    Unbind();
}

void Buffer::Upload(size_t pSize, void* pData)
{
    mSize = pSize;
    Bind();
    glBufferData(mTarget, mSize, pData, mUsage);
    Unbind();
}

void Buffer::PartialUpload(size_t pOffset, size_t pSize, const void* pData)
{
    //std::cout << "partial upload " << pSize << std::endl;
    Bind();
    glBufferSubData(mTarget, pOffset, pSize, pData);
    Unbind();
}

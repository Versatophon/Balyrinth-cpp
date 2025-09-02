#pragma once

#include "Bindable.h"

#include <GL/glew.h>
#include <cstdint>

#include "Shader.h"

enum class BufferUsage
{
    Stream,//modified once and used at most few times
    Static,//modified once and used many times
    Dynamic,//modified repeatedly and used many times
};

class Buffer :public Bindable
{
public:
	Buffer(uint32_t pTarget, size_t pSize, uint32_t pUsage, void* pData = nullptr);

    virtual ~Buffer();

    void Bind() const;

    void Debind() const;

    void Upload(size_t pSize, void* pData);

    void PartialUpload(size_t pOffset, size_t pSize, void* pData);

protected:
    uint32_t mId = 0;
    uint32_t mTarget = 0;
    size_t mSize;//Do we need to keep size
    uint32_t mUsage;
    uint32_t mTargetBinding = 0;
    mutable uint32_t mPreviousId = 0;
};

class ArrayBuffer :public Buffer
{
public:
    ArrayBuffer(uint32_t pSize, BufferUsage pBufferUsage, void* pData = nullptr);
};

class IndexBuffer :public Buffer
{
public:
    IndexBuffer(uint32_t pSize, BufferUsage pBufferUsage, void* pData = nullptr);
};

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
};

class Vao :public Bindable
{
public:
    void Init(ShaderProgram* pShaderProgram, ArrayBuffer** pArrayBuffers);

    void Deinit();

    void Bind() const;

    void Debind() const;

private:
    uint32_t mId = 0;
};

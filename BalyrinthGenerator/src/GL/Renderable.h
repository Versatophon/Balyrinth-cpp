#pragma once

#include <cstdint>

#include <vector>

class ArrayBuffer;
class ShaderProgram;
class Vao;

//TODO: look at glCopyBufferSubData
class Renderable
{
public:
    Renderable(ShaderProgram* pShader, size_t pBufferCount, size_t* pBufferItemSize, uint32_t pItemCount);
    void Draw() const;

    void SetItemCount(size_t pItemCount);

    void Append(size_t pBufferIndex, size_t pItemCount, void* pBuffer);
    void Update(size_t pBufferIndex, size_t pBeginIndex, size_t pItemCount, void* pBuffer);

    size_t GetVertexCount() const;
    size_t GetMaxVertexCount() const;

private:
    std::vector<ArrayBuffer*> mArrayBuffers;
    std::vector<size_t> mArrayItemSize;
    std::vector<size_t> mCurrentPositionInBuffer;
    Vao* mVertexArrayObject = nullptr;
    size_t mItemCount = 0;
};


#include "Renderable.h"

#include <GL/glew.h>

#include "Binder.h"

#include "Buffers/ArrayBuffer.h"
#include "Buffers/Vao.h"

Renderable::Renderable(ShaderProgram* pShader, size_t pBufferCount, size_t* pBufferItemSize, uint32_t pItemCount):
    mItemCount(pItemCount)
{
    mVertexArrayObject = new Vao;

    for (size_t i = 0; i < pBufferCount; ++i)
    {
        mArrayItemSize.push_back(pBufferItemSize[i]);
        mArrayBuffers.push_back(new ArrayBuffer(mItemCount * pBufferItemSize[i], BufferUsage::Dynamic));
        mCurrentPositionInBuffer.push_back(0);
    }
    
    mVertexArrayObject->Init();
    mVertexArrayObject->ConfigureArrayBuffers(pShader, mArrayBuffers.data());
}

void Renderable::Draw() const
{
    size_t lSizeToDraw = SIZE_MAX;
    for (size_t i = 0; i < mCurrentPositionInBuffer.size(); ++i)
    {
        lSizeToDraw = std::min(lSizeToDraw, mCurrentPositionInBuffer[i]);
    }

    if (lSizeToDraw > 0)
    {
        Binder lVaoBinder(*mVertexArrayObject);
        glDrawArrays(GL_TRIANGLES, 0, lSizeToDraw);
    }
}

void Renderable::SetItemCount(size_t pItemCount)
{
    for (size_t i = 0; i < mArrayItemSize.size(); ++i)
    {
        mArrayBuffers[i]->SetSize(pItemCount * mArrayItemSize[i]);
        mCurrentPositionInBuffer[i] = 0;
    }
}

void Renderable::Append(size_t pBufferIndex, size_t pItemCount, void* pBuffer)
{
    mArrayBuffers[pBufferIndex]->PartialUpload(mCurrentPositionInBuffer[pBufferIndex] * mArrayItemSize[pBufferIndex], pItemCount * mArrayItemSize[pBufferIndex], pBuffer);
    mCurrentPositionInBuffer[pBufferIndex] += pItemCount;
}

void Renderable::Update(size_t pBufferIndex, size_t pBeginIndex, size_t pItemCount, void* pBuffer)
{
    mArrayBuffers[pBufferIndex]->PartialUpload(pBeginIndex * mArrayItemSize[pBufferIndex], pItemCount * mArrayItemSize[pBufferIndex], pBuffer);
    mCurrentPositionInBuffer[pBufferIndex] = std::max((pBeginIndex+ pItemCount)/**mArrayItemSize[pBufferIndex]*/, mCurrentPositionInBuffer[pBufferIndex]);
}

size_t Renderable::GetVertexCount() const
{
    size_t lSizeToDraw = SIZE_MAX;
    for (size_t i = 0; i < mCurrentPositionInBuffer.size(); ++i)
    {
        lSizeToDraw = std::min(lSizeToDraw, mCurrentPositionInBuffer[i]);
    }
    return lSizeToDraw;
}

size_t Renderable::GetMaxVertexCount() const
{
    size_t lSizeToDraw = 0;
    for (size_t i = 0; i < mCurrentPositionInBuffer.size(); ++i)
    {
        lSizeToDraw = std::max(lSizeToDraw, mCurrentPositionInBuffer[i]);
    }
    return lSizeToDraw;
}

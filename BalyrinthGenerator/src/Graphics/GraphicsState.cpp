#include "GraphicsState.h"

#include <gl/glew.h>

void GraphicsState::Init()
{
    mRendererName = (const char*)glGetString(GL_RENDERER);
    int32_t lGpuMemory;
    glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &lGpuMemory);
    if (glGetError() == GL_NO_ERROR)
    {
        mCheckFreeMemoryEnum = GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX;
        glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &mTotalMemory);
    }
    else
    {
        glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, &lGpuMemory);
        if (glGetError() == GL_NO_ERROR)
        {
            mCheckFreeMemoryEnum = GL_TEXTURE_FREE_MEMORY_ATI;
            glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, &mTotalMemory);
        }
    }
}

const char* GraphicsState::GetRendererName() const
{
    return mRendererName.c_str();
}

bool GraphicsState::GetMemoryUsage(float& pMemoryUsageNormalized, const char** pMemoryUsageText) const
{
    if (mTotalMemory > 0)
    {
        int32_t lGpuMemory = 0;
        glGetIntegerv(mCheckFreeMemoryEnum, &lGpuMemory);


        int32_t lMemoryUsedInMB = (mTotalMemory - lGpuMemory) / 1024;
        int32_t lTotalMemoryInMB = mTotalMemory / 1024;

        mTextStateBuffer = std::to_string(lMemoryUsedInMB) + " / " + std::to_string(lTotalMemoryInMB) + " MB";

        *pMemoryUsageText = mTextStateBuffer.c_str();
        pMemoryUsageNormalized = float(lMemoryUsedInMB) / float(lTotalMemoryInMB);

        return true;
    }

    return false;
}

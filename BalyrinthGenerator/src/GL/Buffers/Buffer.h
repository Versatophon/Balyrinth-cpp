#pragma once

#include "../Bindable.h"

#include <cstdint>

class Buffer :public Bindable
{
public:
	Buffer(uint32_t pTarget, size_t pSize, uint32_t pUsage, const void* pData = nullptr);

    virtual ~Buffer();

    void Bind() const;

    void Unbind() const;

    void SetSize(size_t pSize);

    void Upload(size_t pSize, void* pData);

    void PartialUpload(size_t pOffset, size_t pSize, const void* pData);

protected:
    uint32_t mId = 0;
    uint32_t mTarget = 0;
    size_t mSize;//Do we need to keep size
    uint32_t mUsage;
    uint32_t mTargetBinding = 0;
    mutable uint32_t mPreviousId = 0;
};

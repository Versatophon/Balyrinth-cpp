#pragma once

#include "../Maths/IRect.h"

class Viewport
{
public:
    void SetSize(const IVec2& pSize);

    void Activate() const;

private:
    IRect mGeometry = { {0, 0}, {10, 10} };
};

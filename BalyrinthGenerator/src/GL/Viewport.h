#pragma once

#include <Rectanglei.h>

class Viewport
{
public:
    void SetSize(const Vector2i& pSize);

    void Activate() const;

private:
    Rectanglei mGeometry = { {0, 0}, {10, 10} };
};

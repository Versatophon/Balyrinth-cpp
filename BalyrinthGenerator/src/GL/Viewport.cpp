#include "Viewport.h"

#include <GL/glew.h>

void Viewport::SetSize(const Vector2i& pSize)
{
    mGeometry.Size = pSize;
}

void Viewport::Activate() const
{
    glViewport(mGeometry.Position.X, mGeometry.Position.Y, mGeometry.Size.Width, mGeometry.Size.Height);
}

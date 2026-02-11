#pragma once

#include <GL/glew.h>
#include "Buffers.h"

struct Renderable
{
    ArrayBuffer** VertexBuffers = nullptr;
    Vao* VertexArrayObject = nullptr;
    uint32_t VertexCount = 0;
    uint32_t CurrentPositionInBuffer = 0;

    Renderable(ShaderProgram* pShader, uint32_t pVertexCount);

    void Draw() const;
};


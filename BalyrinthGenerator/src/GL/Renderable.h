#pragma once

#include <cstdint>

class ArrayBuffer;
class ShaderProgram;
class Vao;

struct Renderable
{
    ArrayBuffer** VertexBuffers = nullptr;
    Vao* VertexArrayObject = nullptr;
    uint32_t VertexCount = 0;
    uint32_t CurrentPositionInBuffer = 0;

    Renderable(ShaderProgram* pShader, uint32_t pVertexCount);

    void Draw() const;
};


#pragma once

#include "Enums.h"

#include <vector>

#include <Vector3f.h>

class ShaderProgram;
class Vao;
class ArrayBuffer;
class IndexBuffer;

class RenderableMesh
{
public:
    RenderableMesh(ShaderProgram* pShaderProgram, const std::vector<Vector3f>& pVertices, const std::vector<uint8_t>& pColorIndices, const std::vector<uint16_t> pGeometryIndices, GeometryType pGeometryType);

    ~RenderableMesh();

    void Render();

protected:
    ArrayBuffer** mVertexBuffers = nullptr;
    IndexBuffer* mIndices = nullptr;
    Vao* mVao = nullptr;

    GeometryType mGeometryType = GeometryType::Triangles;
    uint32_t mElementCount = 0;
};

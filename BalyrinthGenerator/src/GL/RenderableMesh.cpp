#include "RenderableMesh.h"
#include <GL/glew.h>

#include "Binder.h"

#include "Buffers/ArrayBuffer.h"
#include "Buffers/IndexBuffer.h"
#include "Buffers/Vao.h"

RenderableMesh::RenderableMesh(ShaderProgram* pShaderProgram, const std::vector<Vector3f>& pVertices, const std::vector<uint8_t>& pColorIndices, const std::vector<uint16_t> pGeometryIndices, GeometryType pGeometryType)
{
    mVao = new Vao;

    mVertexBuffers = new ArrayBuffer * [2];
    mVertexBuffers[0] = new ArrayBuffer(pVertices.size() * sizeof(float) * 3, BufferUsage::Dynamic, pVertices.data());
    mVertexBuffers[1] = new ArrayBuffer(pColorIndices.size() * sizeof(uint8_t), BufferUsage::Dynamic, pColorIndices.data());

    mVao->Init();
    mVao->ConfigureArrayBuffers(pShaderProgram, mVertexBuffers);

    mGeometryType = pGeometryType;
    mElementCount = pGeometryIndices.size();
    mIndices = new IndexBuffer(mElementCount * sizeof(uint16_t), BufferUsage::Dynamic, pGeometryIndices.data());
}

RenderableMesh::~RenderableMesh()
{
    delete mIndices;
    delete mVertexBuffers[1];
    delete mVertexBuffers[0];
    delete mVertexBuffers;
    delete mVao;
}

void RenderableMesh::Render()
{
    Binder lVaoBinder(*mVao);
    Binder lIndicesBinder(*mIndices);

    glDrawElements(uint32_t(mGeometryType), mElementCount, GL_UNSIGNED_SHORT, 0);
}

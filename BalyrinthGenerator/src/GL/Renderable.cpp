#include "Renderable.h"

#include "Binder.h"

Renderable::Renderable(ShaderProgram* pShader, uint32_t pVertexCount):
    VertexCount(pVertexCount)
{
    VertexArrayObject = new Vao;
    VertexBuffers = new ArrayBuffer * [2];
    VertexBuffers[0] = new ArrayBuffer(VertexCount * sizeof(float) * 3, BufferUsage::Dynamic, nullptr);
    VertexBuffers[1] = new ArrayBuffer(VertexCount * sizeof(uint8_t), BufferUsage::Dynamic, nullptr);
    VertexArrayObject->Init();
    VertexArrayObject->ConfigureArrayBuffers(pShader, VertexBuffers);
}

void Renderable::Draw() const
{
    Binder lVaoBinder(*VertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, CurrentPositionInBuffer / (sizeof(float) * 3));
}

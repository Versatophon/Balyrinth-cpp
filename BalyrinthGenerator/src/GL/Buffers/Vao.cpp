#include "Vao.h"

#include <GL/glew.h>

#include "ArrayBuffer.h"
#include "../ShaderProgram.h"

void Vao::Init()
{
    if (mId == 0)
    {
        glGenVertexArrays(1, &mId);
    }
}

void Vao::Deinit()
{
    glDeleteVertexArrays(1, &mId);
    mId = 0;
}

void Vao::ConfigureArrayBuffers(ShaderProgram* pShaderProgram, ArrayBuffer** pArrayBuffers)
{
    Bind();

    //TODO: prefer interleaved buffers
    uint32_t lAttributeCount = pShaderProgram->GetAttributeCount();
    for (uint32_t i = 0; i < lAttributeCount; ++i)
    {
        pArrayBuffers[i]->Bind();
        glEnableVertexAttribArray(pShaderProgram->GetAttribute(i));
        switch (pShaderProgram->GetAttributeType(i))
        {
        case AttributeType::Float:
            glVertexAttribPointer(pShaderProgram->GetAttribute(i), 3, GL_FLOAT, false, 3 * sizeof(GLfloat), 0);
            break;
        case AttributeType::Integer:
            glVertexAttribIPointer(pShaderProgram->GetAttribute(i), 1, GL_UNSIGNED_BYTE, 1 * sizeof(GLubyte), 0);
            break;
        case AttributeType::Double:
            glVertexAttribIPointer(pShaderProgram->GetAttribute(i), 1, GL_DOUBLE, 1 * sizeof(GLdouble), 0);
            break;
        }
        pArrayBuffers[i]->Unbind();
    }

    Unbind();
}

void Vao::Bind() const
{
    glBindVertexArray(mId);
}

void Vao::Unbind() const
{
    glBindVertexArray(0);
}

#pragma once

#include "../Bindable.h"

#include <cstdint>

class ArrayBuffer;
class ShaderProgram;

class Vao :public Bindable
{
public:
    void Init();
    void Deinit();

    void ConfigureArrayBuffers(ShaderProgram* pShaderProgram, ArrayBuffer** pArrayBuffers);

    void Bind() const override;
    void Unbind() const override;

private:
    uint32_t mId = 0;
};

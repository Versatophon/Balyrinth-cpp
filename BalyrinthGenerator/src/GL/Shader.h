#pragma once

#include <cstdint>

#include "Enums.h"

class Ubo;

class Shader
{
public:
	Shader(ShaderType pType);
	~Shader();

	ShaderType Type() const;
	uint32_t Id() const;

	void LoadFromString(const char* pSource);
	void LoadFromFile(const char* pFilename);

private:
	ShaderType mType;
	uint32_t mIntType;
	uint32_t mId = 0;
};

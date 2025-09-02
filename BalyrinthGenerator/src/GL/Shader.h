#pragma once


#include <unordered_map>
#include <string>
#include <vector>
#include <cstdint>

class Ubo;

enum class ShaderType 
{ 
	VERTEX_SHADER,
	FRAGMENT_SHADER,
	GEOMETRY_SHADER
};

enum class AttributeType
{
	FLOAT,
	INTEGER,
	DOUBLE
};

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

class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();

	void AttachShader(Shader* pShader);
	void Link();

	void Use() const;
	void Deuse() const;

	void LinkUbo(Ubo* pUbo);

	//TODO: also add data type
	void AddAttribute(const char* pAttribute, AttributeType pType);
	void AddUniform(const char* pUniform);

	uint32_t GetAttributeCount() const;
	uint32_t GetAttribute(uint32_t pIndex) const;
	AttributeType GetAttributeType(uint32_t pIndex) const;
	uint32_t GetUniformCount() const;
	uint32_t GetUniform(uint32_t pIndex) const;

	//Should be done in another way
	void UpdateUniform(const char* pUniform, uint32_t pValue) const;
private:
	uint32_t mId = 0;
	mutable uint32_t mPreviousId = 0;

	std::vector<uint32_t> mAttributes;
	std::vector<AttributeType> mAttributeTypes;
	std::vector<uint32_t> mUniforms;
};

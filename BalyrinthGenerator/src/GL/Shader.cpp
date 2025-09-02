#include <GL/glew.h>

#include "Shader.h"
#include "Buffers.h"

#include <iostream>
#include <fstream>
#include <string>

static std::unordered_map< ShaderType, uint32_t> sShaderTypeToInternal =
{
	{ShaderType::FRAGMENT_SHADER, GL_FRAGMENT_SHADER},
	{ShaderType::VERTEX_SHADER, GL_VERTEX_SHADER},
	{ShaderType::GEOMETRY_SHADER, GL_GEOMETRY_SHADER},
};

Shader::Shader(ShaderType pType):
	mType(pType),
	mIntType(sShaderTypeToInternal[mType]),
	mId(glCreateShader(mIntType))
{
}

Shader::~Shader()
{
	glDeleteShader(mId);
}

ShaderType Shader::Type() const
{
	return mType;
}

uint32_t Shader::Id() const
{
	return mId;
}

void Shader::LoadFromString(const char* pSource)
{
	//const char* ptmp = source;
	glShaderSource(mId, 1, &pSource, NULL);

	//check whether the shader loads fine
	GLint lStatus;
	glCompileShader(mId);
	glGetShaderiv(mId, GL_COMPILE_STATUS, &lStatus);
	if (lStatus == GL_FALSE) {
		GLint lInfoLogLength;
		glGetShaderiv(mId, GL_INFO_LOG_LENGTH, &lInfoLogLength);
		GLchar* lInfoLog = new GLchar[lInfoLogLength];
		glGetShaderInfoLog(mId, lInfoLogLength, NULL, lInfoLog);
		std::cerr << "Compile log: " << lInfoLog << std::endl;
		delete[] lInfoLog;
	}
}

void Shader::LoadFromFile(const char* pFilename)
{
	std::ifstream lFileStream;
	lFileStream.open(pFilename, ::std::ios_base::in);
	if (lFileStream) {
		std::string lBuffer(::std::istreambuf_iterator<char>(lFileStream), (::std::istreambuf_iterator<char>()));
		//copy to source
		LoadFromString(lBuffer.c_str());
	}
	else {
		::std::cerr << "Error loading shader: " << pFilename << ::std::endl;
	}
}

ShaderProgram::ShaderProgram():
	mId(glCreateProgram())
{
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(mId);
}

void ShaderProgram::AttachShader(Shader* pShader)
{
	glAttachShader(mId, pShader->Id());

}

void ShaderProgram::Link()
{
	//link and check whether the program links fine
	GLint lStatus;
	glLinkProgram(mId);
	glGetProgramiv(mId, GL_LINK_STATUS, &lStatus);
	if (lStatus == GL_FALSE) 
	{
		GLint lInfoLogLength;

		glGetProgramiv(mId, GL_INFO_LOG_LENGTH, &lInfoLogLength);
		GLchar* lInfoLog = new GLchar[lInfoLogLength];
		glGetProgramInfoLog(mId, lInfoLogLength, nullptr, lInfoLog);

		std::cerr << lInfoLogLength << " - Link log: " << ((long*)lInfoLog)[0] << std::endl;
		delete[] lInfoLog;
	}
}

void ShaderProgram::Use() const
{
	glGetIntegerv(GL_CURRENT_PROGRAM, (int32_t*)&mPreviousId);
	glUseProgram(mId);
}

void ShaderProgram::Deuse() const
{
	glUseProgram(mPreviousId);
}

void ShaderProgram::LinkUbo(Ubo* pUbo)
{
	glUniformBlockBinding(mId, glGetUniformBlockIndex(mId, pUbo->GetBindingName()), pUbo->GetBindingPoint());
}

void ShaderProgram::AddAttribute(const char* pAttribute, AttributeType pType)
{
	uint32_t lAttributeId = glGetAttribLocation(mId, pAttribute);
	mAttributes.push_back(lAttributeId);
	mAttributeTypes.push_back(pType);
}

void ShaderProgram::AddUniform(const char* pUniform)
{
	uint32_t lUniformId = glGetUniformLocation(mId, pUniform);
	mUniforms.push_back(lUniformId);
}

uint32_t ShaderProgram::GetAttributeCount() const
{
	return mAttributes.size();
}

uint32_t ShaderProgram::GetAttribute(uint32_t pIndex) const
{
	return mAttributes[pIndex];
}

AttributeType ShaderProgram::GetAttributeType(uint32_t pIndex) const
{
	return mAttributeTypes[pIndex];
}

uint32_t ShaderProgram::GetUniformCount() const
{
	return mUniforms.size();
}

uint32_t ShaderProgram::GetUniform(uint32_t pIndex) const
{
	return mUniforms[pIndex];
}

void ShaderProgram::UpdateUniform(const char* pUniform, uint32_t pValue) const
{
	glUniform1ui(glGetUniformLocation(mId, pUniform), pValue);
}

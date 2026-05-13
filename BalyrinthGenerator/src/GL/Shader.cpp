#include "Shader.h"

#include <iostream>
#include <fstream>
#include <unordered_map>

#include <GL/glew.h>

static std::unordered_map< ShaderType, uint32_t> sShaderTypeToInternal =
{
	{ShaderType::FragmentShader, GL_FRAGMENT_SHADER},
	{ShaderType::VertexShader, GL_VERTEX_SHADER},
	{ShaderType::GeometryShader, GL_GEOMETRY_SHADER},
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
	if (lStatus == GL_FALSE)
	{
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
	if (lFileStream)
	{
		std::string lBuffer(::std::istreambuf_iterator<char>(lFileStream), (::std::istreambuf_iterator<char>()));
		//copy to source
		LoadFromString(lBuffer.c_str());
	}
	else 
	{
		std::cerr << "Error loading shader: " << pFilename << ::std::endl;
	}
}

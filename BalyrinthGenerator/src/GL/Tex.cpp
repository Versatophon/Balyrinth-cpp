#include "Tex.h"

#include <GL/glew.h>

void Tex::Bind() const
{
	glGetIntegerv(GL_TEXTURE_BINDING_2D, (int32_t*)&mPreviousId);
	glBindTexture(GL_TEXTURE_2D, Id);
}

void Tex::Debind() const
{
	glBindTexture(GL_TEXTURE_2D, mPreviousId);
}

void Tex::Generate()
{
	if (Id != 0)
	{
		glDeleteTextures(1, &Id);
	}
	glGenTextures(1, &Id);
}

void Tex::Destroy()
{
	glDeleteTextures(1, &Id);
	Id = 0;
}

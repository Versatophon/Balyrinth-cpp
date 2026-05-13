#include "Texture.h"

#include <GL/glew.h>

void Texture::Bind() const
{
	glGetIntegerv(GL_TEXTURE_BINDING_2D, (int32_t*)&mPreviousId);
	glBindTexture(GL_TEXTURE_2D, Id);
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, mPreviousId);
}

void Texture::Generate()
{
	if (Id != 0)
	{
		glDeleteTextures(1, &Id);
	}
	glGenTextures(1, &Id);
}

void Texture::Destroy()
{
	glDeleteTextures(1, &Id);
	Id = 0;
}

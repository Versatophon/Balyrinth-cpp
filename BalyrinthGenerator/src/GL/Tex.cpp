#include "Tex.h"

#include <GL/glew.h>

void Tex::Bind() const
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, Id);
}

void Tex::Debind() const
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
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

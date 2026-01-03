#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>

#include"shaderClass.h"

class Texture
{
	public:
		Texture(const char* image, GLenum texType, GLenum slot, GLenum pixelType);
		~Texture();

		GLuint ID = 0;
		GLenum type;

		void texUnit(Shader& shader, const char* uniform, GLuint unit);

		void Bind();
		void Unbind();
		void Delete();
};
#endif

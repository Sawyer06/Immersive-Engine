#include"Texture.h"

Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum pixelType)
{
	type = texType;

	int widthImg, heightImg, numColCh;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);
	if (!bytes)
	{
		std::cerr << "IMAGE_LOAD_ERROR failed to load texture: " << image << std::endl;
		return;
	}
	GLenum format;
	if (numColCh == 4) // Image includes alpha channel.
	{
		format = GL_RGBA;
	}
	else if (numColCh == 3) // Image does not include alpha channel.
	{
		format = GL_RGB;
	}
	else // Image type is unsupported.
	{
		std::cerr << "IMAGE_LOAD_ERROR unsupported number of color channels: " << numColCh << std::endl;
		stbi_image_free(bytes);
		return;
	}

	glGenTextures(1, &ID);
	glActiveTexture(slot);
	glBindTexture(texType, ID);

	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(texType, 0, format, widthImg, heightImg, 0, format, pixelType, bytes);
	glGenerateMipmap(texType);

	stbi_image_free(bytes);
	glBindTexture(texType, 0);
}

Texture::~Texture()
{
	Delete();
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	shader.Activate();
	glUniform1i(texUni, unit);
}

void Texture::Bind()
{
	glBindTexture(type, ID);
}

void Texture::Unbind()
{
	glBindTexture(type, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}
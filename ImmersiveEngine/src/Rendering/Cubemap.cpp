#include"Cubemap.h"

namespace ImmersiveEngine::Rendering
{
	Cubemap::Cubemap(std::vector<Texture> textures) : 
		m_textures(textures)
	{
		setTextures(m_textures);
		buildSkybox();
	}

	Cubemap::~Cubemap()
	{
		m_VAO.Delete();
	}

	std::vector<Vertex> vertices =
	{
		Vertex{ImmersiveEngine::Math::Vector3(-1.0f, -1.0f, 1.0f)},
		Vertex{ImmersiveEngine::Math::Vector3(1.0f, -1.0f, 1.0f)},
		Vertex{ImmersiveEngine::Math::Vector3(1.0f, -1.0f, -1.0f)},
		Vertex{ImmersiveEngine::Math::Vector3(-1.0f, -1.0f, -1.0f)},
		Vertex{ImmersiveEngine::Math::Vector3(-1.0f, 1.0f, 1.0f)},
		Vertex{ImmersiveEngine::Math::Vector3(1.0f, 1.0f, 1.0f)},
		Vertex{ImmersiveEngine::Math::Vector3(1.0f, 1.0f, -1.0f)},
		Vertex{ImmersiveEngine::Math::Vector3(-1.0f, 1.0f, -1.0f)}
	};
	std::vector<GLuint> indices =
	{
		1, 2, 6,
		6, 5, 1,

		0, 4, 7,
		7, 3, 0,

		4, 5, 6,
		6, 7, 4,

		0, 3, 2,
		2, 1, 0,

		0, 1, 5,
		5, 4, 0,

		3, 7, 6,
		6, 2, 3
	};

	void Cubemap::buildSkybox()
	{
		m_VAO.Bind();

		VBO VBO(vertices);
		EBO EBO(indices);
		EBO.Bind();
		/// Assign buffer objects to layout locations in the VAO (buffer object, layout index, number of elements).
		m_VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0); // Vertex positions

		m_VAO.Unbind();
		VBO.Unbind();

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	void Cubemap::setTextures(std::vector<Texture> textures)
	{
		m_textures = textures;
		buildSkybox();
	}

	void Cubemap::draw(Shader& shaderProgram)
	{
		glDepthFunc(GL_LEQUAL);

		m_VAO.Bind();
		glActiveTexture(GL_TEXTURE0);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		m_VAO.Unbind();

		glDepthFunc(GL_LESS);
	}
}
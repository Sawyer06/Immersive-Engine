#include"Cubemap.h"

namespace ImmersiveEngine::Rendering
{
	Cubemap::Cubemap(std::vector<std::string> faces) : 
		m_faces(faces)
	{
		setFaceTextures(m_faces);
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

	uint32_t Cubemap::loadCubemap(std::vector<std::string> faces)
	{
		uint32_t ID;
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

		int width, height, numColCh;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char* bytes = stbi_load(faces[i].c_str(), &width, &height, &numColCh, 0);
			if (bytes)
			{
				GLenum format = GL_RGB;
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
					return 0;
				}

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, format, width, height, 0, format, GL_UNSIGNED_BYTE, bytes);

				stbi_image_free(bytes);
			}
			else
			{
				std::cout << "IMAGE_LOAD_ERROR failed to load texture: " << faces[i] << std::endl;
				stbi_image_free(bytes);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return ID;
	}

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

		m_cubemapTexture = loadCubemap(m_faces);
	}

	void Cubemap::setFaceTextures(std::vector<std::string> faces)
	{
		m_faces = faces;
		buildSkybox();
	}

	void Cubemap::draw(Shader& shaderProgram)
	{
		m_VAO.Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapTexture);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		m_VAO.Unbind();
	}
}
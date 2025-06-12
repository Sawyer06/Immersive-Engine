#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include<glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Rendering/shaderClass.h"
#include"../Rendering/VAO.h"
#include"../Rendering/VBO.h"
#include"../Rendering/EBO.h"
#include"../Rendering/Texture.h"

class Mesh
{
	private:
		VAO m_VAO;
		VBO m_VBO;
		EBO m_EBO;
		Texture* m_texture;
		int m_indexCount;
	public:
		Mesh(GLfloat* vertices, GLsizeiptr verticesSize, GLuint* indices, GLsizeiptr indicesSize);
		~Mesh();

		void draw(Shader& shaderProgram);
		void setTexture(Texture* texture);

		void dump();
};
#endif

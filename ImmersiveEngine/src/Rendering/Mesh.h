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
		std::vector<Vertex> m_vertices;
		std::vector<GLuint> m_indices;
		Texture* m_texture;
		
		VAO m_VAO;
		VBO m_VBO;
		EBO m_EBO;

		void buildMesh();
	public:
		Mesh(std::vector<Vertex>& vertices, std::vector <GLuint>& indices);
		~Mesh();

		void draw(Shader& shaderProgram);
		void setTexture(Texture* texture);

		static Mesh generateSquare(float length);
		static Mesh generateCircle(float radius, uint32_t segments);
		static Mesh generatePlane(float length, float width);
		static Mesh generateCube(float length);

		void dump();
};
#endif

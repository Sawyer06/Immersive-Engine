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
#include"../Math/Math.h"
#include"../Settings.h"

class Mesh
{
	private:
		std::vector<Vertex> m_vertices;
		std::vector<GLuint> m_indices;
		std::shared_ptr<Texture> m_texture;
		
		VAO m_VAO;
	public:
		Mesh(std::vector<Vertex>& vertices, std::vector <GLuint>& indices);
		Mesh(const Mesh& mesh);
		~Mesh();

		float textureScale = 1.0f;
		ImmersiveEngine::Math::Vector2 textureOffset;

		void buildMesh();

		void draw(Shader& shaderProgram);
		void setTexture(std::shared_ptr<Texture>& texture);

		void addUVOffset(ImmersiveEngine::Math::Vector2 offset);

		ImmersiveEngine::Math::Vector3 getVertexPosition(const int index);
		void setVertexPosition(const int index, const ImmersiveEngine::Math::Vector3 pos);
		ImmersiveEngine::Math::Vector3 getNormalDirection(const int index);

		uint32_t getVerticesCount();

		void dump();

		static Mesh generateSquare(const float length);
		static Mesh generateCircle(const float radius, uint32_t segments);
		static Mesh generatePlane(const float length, const float width);
		static Mesh generateCube(const float length);
		static Mesh generateSquarePyramid(const float length, const float height);
		static Mesh generateSphere(const float radius, uint32_t sectorCount, uint32_t stackCount);
};
#endif

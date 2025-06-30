#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glad/glad.h>
#include<vector>
#include"../Math/Vector3.h"
#include"../Math/Vector2.h"

struct Vertex
{
	ImmersiveEngine::Math::Vector3 position;
	ImmersiveEngine::Math::Vector3 normal;
	ImmersiveEngine::Math::Vector3 color;
	ImmersiveEngine::Math::Vector2 uvOffset;
};

class VBO
{
	public:
		GLuint ID;
        
		VBO(std::vector<Vertex>& vertices);
        
		void Bind();
		void Unbind();
		void Delete();
};

#endif

#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glad/glad.h>
#include<vector>
#include"../Math/Vector3.h"
#include"../Math/Vector2.h"


namespace ImmersiveEngine::Rendering
{
	#define MAX_BONE_INFLUENCE 4
	#define MAX_BONES 100

	struct Vertex
	{
		Math::Vector3 position;
		Math::Vector3 normal;
		Math::Vector3 color;
		Math::Vector2 uvOffset;

		int boneIDs[MAX_BONE_INFLUENCE] = { -1, -1, -1, -1 };
		float weights[MAX_BONE_INFLUENCE];
	};

	class VBO
	{
		public:
			GLuint ID;

			VBO(std::vector<Vertex>& vertices);
			~VBO();

			void Bind();
			void Unbind();
			void Delete();
	};
}
#endif

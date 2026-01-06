#ifndef CUBEMAP_CLASS_H
#define CUBEMAP_CLASS_H

#include"VAO.h"
#include"EBO.h"
#include"Texture.h"
#include"../Math/Vector3.h"

namespace ImmersiveEngine::Rendering
{
	class Cubemap
	{
		private:
			std::vector<Texture> m_textures;

			VAO m_VAO;

			void buildSkybox();
		public:
			Cubemap(std::vector<Texture> textures);
			~Cubemap();

			void setTextures(std::vector<Texture> textures);
			void draw(Shader& shaderProgram);
	};
}
#endif

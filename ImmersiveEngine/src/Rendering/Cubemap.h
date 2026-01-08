#ifndef CUBEMAP_CLASS_H
#define CUBEMAP_CLASS_H

#include<stb/stb_image.h>
#include"VAO.h"
#include"EBO.h"
#include"shaderClass.h"
#include"../Math/Vector3.h"

namespace ImmersiveEngine::Rendering
{
	class Cubemap
	{
		private:
			std::vector<std::string> m_faces;
			uint32_t m_cubemapTexture;

			VAO m_VAO;

			uint32_t loadCubemap(std::vector<std::string> faces);
			void buildSkybox();
		public:
			Cubemap(std::vector<std::string> faces);
			~Cubemap();

			void setFaceTextures(std::vector<std::string> faces);
			void draw(Shader& shaderProgram);
	};
}
#endif

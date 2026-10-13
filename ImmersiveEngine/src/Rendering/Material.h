#ifndef MATERIAL_CLASS_H
#define MATERIAL_CLASS_H

#include"../Application/Resource.h"
#include"shaderClass.h"
#include"Texture.h"
#include"../Math/Vector2.h"
#include"../Math/Vector3.h"
#include"../Settings.h"

namespace ImmersiveEngine::Rendering
{
	class Material : public Resource
	{
		public:
			Material(Shader& shader, std::shared_ptr<Texture> diffuseTexture);
			~Material() = default;

			Shader& shader;

			ImmersiveEngine::Math::Vector3 mainColor = Settings::g_defaultMeshColor;

			std::shared_ptr<Texture> diffuseTexture;
			float textureScale = 1.0f;
			ImmersiveEngine::Math::Vector2 textureOffset;

			float shininess = 32.0f;
	};
}
#endif
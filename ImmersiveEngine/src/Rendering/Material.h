#ifndef MATERIAL_CLASS_H
#define MATERIAL_CLASS_H

#include"../Application/Resource.h"
#include"shaderClass.h"
#include"Texture.h"
#include"../Math/Vector2.h"

namespace ImmersiveEngine::Rendering
{
	class Material : public Resource
	{
		public:
			Material(Shader& shader, std::shared_ptr<Texture> texture);
			~Material() = default;

			Shader& shader;

			std::shared_ptr<Texture> texture;
			float textureScale = 1.0f;
			ImmersiveEngine::Math::Vector2 textureOffset;

		private:

	};
}
#endif
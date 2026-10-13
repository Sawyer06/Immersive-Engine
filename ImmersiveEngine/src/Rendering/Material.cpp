#include"Material.h"

namespace ImmersiveEngine::Rendering
{
	Material::Material(Shader& shader, std::shared_ptr<Texture> diffuseTexture = nullptr) :
		shader(shader), diffuseTexture(diffuseTexture)
	{

	}
}
#include"Material.h"

namespace ImmersiveEngine::Rendering
{
	Material::Material(Shader& shader, std::shared_ptr<Texture> texture = nullptr) :
		shader(shader), texture(texture)
	{

	}
}
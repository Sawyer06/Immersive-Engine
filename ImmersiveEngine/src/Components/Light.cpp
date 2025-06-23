#include"Light.h"

namespace ImmersiveEngine::cbs
{
	//std::vector<std::type_index> Component::dependencies = { typeid(Space) };

	Light::Light(Object* obj) : Component(obj), color(ImmersiveEngine::Math::Vector3(1,1,1)) { }
	Light::Light(Object* obj, const ImmersiveEngine::Math::Vector3 color) : Component(obj), color(color) { }

	void Light::refreshLight(Shader& shaderProgram, const ImmersiveEngine::Math::Vector3 position)
	{
		shaderProgram.setVec4("lightColor", glm::vec4(color.x / 255, color.y / 255, color.z / 255, 1.0f));
		shaderProgram.setVec3("lightPos", glm::vec3(position.x, position.y, position.z));
	}
}
#include"Light.h"

namespace ImmersiveEngine::cbs
{
	Light::Light(Object* obj) :
		Component(obj), mainColor(ImmersiveEngine::Math::Vector3(255, 255, 255)), intensity(1.0f),
		ambient(mainColor, 0.2f), diffuse(mainColor, 1.0f), specular(mainColor, 0.3f)
	{ }
	Light::Light(Object* obj, const ImmersiveEngine::Math::Vector3 mainColor, const float intensity) : 
		Component(obj), mainColor(mainColor), intensity(intensity),
		ambient(mainColor, 0.2f), diffuse(mainColor, 1.0f), specular(mainColor, 0.3f)
	{ }

	/// Update lighting vales on the shader.
	void Light::refreshLight(Shader& shaderProgram, const ImmersiveEngine::Math::Vector3 position)
	{
		shaderProgram.setVec3("lightColor", glm::vec3(mainColor.x / 255.0f, mainColor.y / 255.0f, mainColor.z / 255.0f));
		shaderProgram.setVec3("lightPos", glm::vec3(position.x, position.y, position.z));
		shaderProgram.setFloat("shininess", shininess);

		shaderProgram.setFloat("constant", 1.0f);
		shaderProgram.setFloat("linear", 1.0f);
		shaderProgram.setFloat("quadratic", 0.01f);

		// Ambient light
		if (useGlobalLight)
		{
			ImmersiveEngine::Math::Vector3 gColor = ImmersiveEngine::Settings::g_ambientLightColor;
			float gIntensity = ImmersiveEngine::Settings::g_ambientLightIntensity;
			shaderProgram.setVec3("ambient.color", glm::vec3(gColor.x / 255.0f, gColor.y / 255.0f, gColor.z / 255.0f));
			shaderProgram.setVec3("ambient.intensity", glm::vec3(gIntensity));
		}
		else
		{
			shaderProgram.setVec3("ambient.color", glm::vec3(ambient.color.x / 255.0f, ambient.color.y / 255.0f, ambient.color.z / 255.0f));
			shaderProgram.setVec3("ambient.intensity", glm::vec3(ambient.intensity));
		}

		// Diffuse light
		shaderProgram.setVec3("diffuse.color", glm::vec3(diffuse.color.x / 255.0f, diffuse.color.y / 255.0f, diffuse.color.z / 255.0f));
		shaderProgram.setVec3("diffuse.intensity", glm::vec3(diffuse.intensity));

		// Specular light
		shaderProgram.setVec3("specular.color", glm::vec3(specular.color.x / 255.0f, specular.color.y / 255.0f, specular.color.z / 255.0f));
		shaderProgram.setVec3("specular.intensity", glm::vec3(specular.intensity));
	}
}
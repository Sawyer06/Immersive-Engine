#ifndef LIGHTING_MANAGER_CLASS_H
#define LIGHTING_MANAGER_CLASS_H

#define MAX_LIGHTS 8

#include"../Objects/Object.h"
#include"../Components/Light.h"

namespace ImmersiveEngine::cbs
{
	class LightingManager
	{
		private:
			std::vector<Light> m_lights;
		public:
			LightingManager() = default;
			~LightingManager() = default;

			bool useGlobalLight = true;

			void addLight(Light& light);
			void removeLight(uint32_t index);
			Light& getLight(uint32_t index);

			void refreshLights(ImmersiveEngine::Rendering::Shader& shaderProgram);

			void onUpdate(ImmersiveEngine::Rendering::Shader& shaderProgram);
	};
}
#endif
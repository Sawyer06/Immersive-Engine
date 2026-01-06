#ifndef LIGHTING_MANAGER_CLASS_H
#define LIGHTING_MANAGER_CLASS_H

#define MAX_LIGHTS 8

#include"Manager.h"
#include"../Objects/Object.h"
#include"../Components/Light.h"

namespace ImmersiveEngine::cbs
{
	class LightingManager : public Manager<LightingManager>
	{
		friend class Manager<LightingManager>;

		private:
			std::vector<Light> m_lights;

			LightingManager() = default;
			~LightingManager() = default;
		public:
			bool useGlobalLight = true;

			void addLight(Light& light);
			void removeLight(uint32_t index);
			Light& getLight(uint32_t index);

			void refreshLights(ImmersiveEngine::Rendering::Shader& shaderProgram);
	};
}
#endif
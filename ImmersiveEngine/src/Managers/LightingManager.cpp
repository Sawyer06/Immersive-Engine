#include"LightingManager.h"

namespace ImmersiveEngine::cbs
{
	void LightingManager::addLight(Light& light)
	{
		m_lights.push_back(light);
	}
	void LightingManager::removeLight(uint32_t index)
	{
		m_lights.erase(m_lights.begin() + index);
	}
	Light& LightingManager::getLight(uint32_t index)
	{
		return m_lights[index];
	}

	void LightingManager::refreshLights(Shader& shaderProgram)
	{
		for (int i = 0; i < m_lights.size(); ++i)
		{
			m_lights[i].refreshLight(shaderProgram, i);
		}
	}
}
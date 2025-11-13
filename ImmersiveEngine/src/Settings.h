#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>
#include"Math/Vector3.h"

namespace ImmersiveEngine::Settings
{
	inline const uint32_t g_screenWidth = 800;
	inline const uint32_t g_screenHeight = 800;
	
	inline ImmersiveEngine::Math::Vector3 g_ambientLightColor(255, 255, 255);
	inline float g_ambientLightIntensity = 1.0f;

	inline bool g_showWireframe = false;
}
#endif

#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>
#include"Math/Vector3.h"

namespace ImmersiveEngine::Settings
{
	inline uint32_t g_screenLength = 1920;
	inline uint32_t g_screenHeight = 1080;
	
	inline ImmersiveEngine::Math::Vector3 g_ambientLightColor(255, 255, 255);
	inline float g_ambientLightIntensity = 0.3f;

	inline bool g_showWireframe = false;
}
#endif

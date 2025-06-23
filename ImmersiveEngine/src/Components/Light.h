#ifndef LIGHT_CLASS_H
#define LIGHT_CLASS_H

#include"../Objects/Present.h"
#include"Space.h"

namespace ImmersiveEngine::cbs
{
	class Light : public Component
	{
		public:
			Light(Object* obj);
			Light(Object* objm, const ImmersiveEngine::Math::Vector3 color);
			~Light() = default;

			ImmersiveEngine::Math::Vector3 color;

			void refreshLight(Shader& shaderProgram, const ImmersiveEngine::Math::Vector3 position);
	};
}
#endif


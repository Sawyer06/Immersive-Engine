#ifndef LIGHT_CLASS_H
#define LIGHT_CLASS_H

#include"../Objects/Present.h"
#include"Space.h"
#include"../Settings.h"

namespace ImmersiveEngine::cbs
{
	class Light : public Component
	{
		private:
			struct LightProperty
			{
				ImmersiveEngine::Math::Vector3 color;
				float intensity = 1.0f;

				LightProperty(ImmersiveEngine::Math::Vector3 color, float intensity) : color(color), intensity(intensity) { }
			};
		public:
			Light(Object* obj);
			Light(Object* obj, const ImmersiveEngine::Math::Vector3 mainColor, const float intensity);
			~Light() = default;

			float shininess = 32.0f;

			LightProperty ambient;
			LightProperty diffuse;
			LightProperty specular;

			void refreshLight(ImmersiveEngine::Rendering::Shader& shaderProgram, bool useGlobalLight, const int index);
	};
}
#endif


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
				float intensity;

				LightProperty(ImmersiveEngine::Math::Vector3 color, float intensity) : color(color), intensity(intensity) { }
			};
		public:
			Light(Object* obj);
			Light(Object* obj, const ImmersiveEngine::Math::Vector3 mainColor, const float intensity);
			~Light() = default;

			ImmersiveEngine::Math::Vector3 mainColor;
			float intensity;
			float shininess = 32.0f;
			bool useGlobalLight = true;

			LightProperty ambient;
			LightProperty diffuse;
			LightProperty specular;

			void refreshLight(Shader& shaderProgram, const int index);
	};
}
#endif


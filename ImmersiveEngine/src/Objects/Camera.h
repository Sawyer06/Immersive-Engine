#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include"Present.h"

namespace ImmersiveEngine::cbs
{
	class Camera : public Component
	{
		private:
			glm::mat4 m_view;
			glm::mat4 m_proj;
		public:
			Camera(const std::shared_ptr<Present> obj);
			~Camera() = default;

			float fov;
			float nearPlane;
			float farPlane;

			void refreshViewProjection(Shader& shaderProgram, const float aspectRatio);
	};
}
#endif

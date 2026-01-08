#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include"../Objects/Object.h"
#include"Space.h"
#include"../XR/OpenXRManager.h"

namespace ImmersiveEngine::cbs
{
	class Camera : public Component
	{
		private:
			glm::mat4 m_view;
			glm::mat4 m_proj;

			Space* m_ownerSpace;
		public:
			Camera(Object* obj);
			~Camera() = default;

			float fov;
			float nearPlane;
			float farPlane;

			void refreshViewProjection(ImmersiveEngine::Rendering::Shader& shaderProgram, ImmersiveEngine::Rendering::Shader& skyboxShader, const float aspectRatio);
			void refreshViewProjection(ImmersiveEngine::Rendering::Shader& shaderProgram, ImmersiveEngine::Rendering::Shader& skyboxShader, XrView view);

			std::string toString() override;
	};
}
#endif

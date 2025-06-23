#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include"../Objects/Present.h"
#include"Space.h"

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

			void refreshViewProjection(Shader& shaderProgram, const float aspectRatio);

			std::string toString() override;
	};
}
#endif

#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include"Present.h"

class Camera : Present
{
	private:
		glm::mat4 m_view;
		glm::mat4 m_proj;
	public:
		Camera();
		~Camera();

		float fov;
		float nearPlane;
		float farPlane;
		Engine::Math::Vector3 view;

		void updateViewProjection(Shader& shaderProgram, const float aspectRatio);
};
#endif

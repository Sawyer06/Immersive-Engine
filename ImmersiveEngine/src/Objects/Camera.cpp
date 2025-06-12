#include "Camera.h"

namespace ImmersiveEngine::cbs
{
	Camera::Camera() :
		fov(60.0f), nearPlane(0.1f), farPlane(100.0f),
		view(ImmersiveEngine::Math::Vector3(0, 0, -2.0f)),
		m_view(glm::mat4(1.0f)), m_proj(glm::mat4(1.0f)) {
	}

	/// Update the view and projection matrices of the camera.
	void Camera::refreshViewProjection(Shader& shaderProgram, const float aspectRatio)
	{
		m_view = glm::mat4(1.0f);
		m_proj = glm::mat4(1.0f);

		glm::vec3 pos(space.position.x, space.position.y, space.position.z);
		glm::vec3 rot(space.orientation.x, space.orientation.y, space.orientation.z);
		glm::vec3 up(space.up.x, space.up.y, space.up.z);
		m_view = glm::lookAt(pos, pos + rot, up); // Position of the world.
		m_proj = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);

		shaderProgram.setMat4("view", m_view);
		shaderProgram.setMat4("proj", m_proj);
	}
}
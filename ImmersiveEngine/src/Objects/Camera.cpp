#include "Camera.h"

namespace ImmersiveEngine::cbs
{
	Camera::Camera(const std::shared_ptr<Present> obj) :
		Component(obj), fov(60.0f), nearPlane(0.1f), farPlane(100.0f),
		m_view(glm::mat4(1.0f)), m_proj(glm::mat4(1.0f)) 
	{
		auto owner = Component::getOwner();
		owner->space.orientation = ImmersiveEngine::Math::Vector3(0, 0, -1);
	}

	/// Update the view and projection matrices of the camera.
	void Camera::refreshViewProjection(Shader& shaderProgram, const float aspectRatio)
	{
		m_view = glm::mat4(1.0f);
		m_proj = glm::mat4(1.0f);
		
		auto owner = Component::getOwner();
		glm::vec3 pos(owner->space.position.x, owner->space.position.y, owner->space.position.z);
		glm::vec3 rot(glm::radians(owner->space.orientation.x), glm::radians(owner->space.orientation.y), glm::radians(owner->space.orientation.z));
		glm::vec3 up(owner->space.up.x, owner->space.up.y, owner->space.up.z);
		m_view = glm::lookAt(pos, pos + rot, up); // Position of the world.
		m_proj = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);

		shaderProgram.setMat4("view", m_view);
		shaderProgram.setMat4("proj", m_proj);
	}
}
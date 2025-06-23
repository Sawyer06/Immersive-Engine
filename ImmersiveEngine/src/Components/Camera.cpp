#include "Camera.h"

namespace ImmersiveEngine::cbs
{
	std::vector<std::type_index> Component::dependencies = { typeid(Space) };

	Camera::Camera(Object* obj) :
		Component(obj), fov(60.0f), nearPlane(0.1f), farPlane(100.0f),
		m_view(glm::mat4(1.0f)), m_proj(glm::mat4(1.0f)) 
	{
		
		auto owner = Component::getOwner();
		if (!m_ownerSpace)
		{
			m_ownerSpace = owner->getComponent<Space>();
		}
		m_ownerSpace->orientation = ImmersiveEngine::Math::Vector3(0, 0, -1);
	}

	/// Update the view and projection matrices of the camera.
	void Camera::refreshViewProjection(Shader& shaderProgram, const float aspectRatio)
	{
		m_view = glm::mat4(1.0f);
		m_proj = glm::mat4(1.0f);
		
		auto owner = Component::getOwner();
		glm::vec3 pos(m_ownerSpace->position.x, m_ownerSpace->position.y, m_ownerSpace->position.z);
		glm::vec3 rot(glm::radians(m_ownerSpace->orientation.x), glm::radians(m_ownerSpace->orientation.y), glm::radians(m_ownerSpace->orientation.z));
		glm::vec3 up(m_ownerSpace->up.x, m_ownerSpace->up.y, m_ownerSpace->up.z);
		m_view = glm::lookAt(pos, pos + rot, up); // Position of the world.
		m_proj = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);

		shaderProgram.setMat4("view", m_view);
		shaderProgram.setMat4("proj", m_proj);
	}

	std::string Camera::toString()
	{
		std::ostringstream oss;
		oss << Component::toString();
		return oss.str();
	}
}
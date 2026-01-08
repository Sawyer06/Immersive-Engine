#include "Camera.h"

namespace ImmersiveEngine::cbs
{
	Camera::Camera(Object* obj) :
		Component(obj), fov(60.0f), nearPlane(0.1f), farPlane(250.0f),
		m_view(glm::mat4(1.0f)), m_proj(glm::mat4(1.0f)) 
	{
		auto owner = Component::getOwner();
		m_ownerSpace = owner->getComponent<Space>();
		if (m_ownerSpace == nullptr)
		{
			MISORDERED_ERROR;
			return;
		}
		m_ownerSpace->orientation = ImmersiveEngine::Math::Quaternion(0, 0, 0, -1);
	}

	/// Update the view and projection matrices of the camera (flatscreen).
	void Camera::refreshViewProjection(ImmersiveEngine::Rendering::Shader& shaderProgram, ImmersiveEngine::Rendering::Shader& skyboxShader, const float aspectRatio)
	{
		if (m_ownerSpace == nullptr)
		{
			MISORDERED_ERROR;
			return;
		}

		m_view = glm::mat4(1.0f);
		m_proj = glm::mat4(1.0f);

		glm::vec3 forward(0, 0, -1);

		glm::vec3 position(m_ownerSpace->position.x, m_ownerSpace->position.y, m_ownerSpace->position.z);
		glm::quat orientation(m_ownerSpace->orientation.w, m_ownerSpace->orientation.x, m_ownerSpace->orientation.y, m_ownerSpace->orientation.z);
		glm::vec3 rot = orientation * forward;
		glm::vec3 up(m_ownerSpace->up.x, m_ownerSpace->up.y, m_ownerSpace->up.z);
		m_view = glm::lookAt(position, position + rot, up); // Position of the world.
		m_proj = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);

		skyboxShader.setMat4("view", m_view);
		skyboxShader.setMat4("proj", m_proj);

		shaderProgram.setMat4("view", m_view);
		shaderProgram.setMat4("proj", m_proj);
		shaderProgram.setVec3("camPos", glm::vec3(m_ownerSpace->position.x, m_ownerSpace->position.y, m_ownerSpace->position.z));
	}

	glm::mat4 createProjectionMatrixFromXrFovf(const XrFovf& fov, float nearZ, float farZ)
	{
		float left = nearZ * tanf(fov.angleLeft);
		float right = nearZ * tanf(fov.angleRight);
		float bottom = nearZ * tanf(fov.angleDown);
		float top = nearZ * tanf(fov.angleUp);

		if (left >= right || bottom >= top || nearZ >= farZ || nearZ <= 0.0f || farZ <= 0.0f) 
		{
			return glm::mat4(1.0f);
		}

		return glm::frustum(left, right, bottom, top, nearZ, farZ);
	}

	/// Update the view and projection matrices of the camera (XR).
	void Camera::refreshViewProjection(ImmersiveEngine::Rendering::Shader& shaderProgram, ImmersiveEngine::Rendering::Shader& skyboxShader, XrView view)
	{
		if (m_ownerSpace == nullptr)
		{
			MISORDERED_ERROR;
			return;
		}

		m_view = glm::mat4(1.0f);
		m_proj = glm::mat4(1.0f);

		XrPosef pose = view.pose;
		glm::quat hmdOrientation(pose.orientation.w, pose.orientation.x, pose.orientation.y, pose.orientation.z);
		glm::vec3 hmdPosition(pose.position.x, pose.position.y, pose.position.z);

		glm::quat realOrientation(m_ownerSpace->orientation.w, m_ownerSpace->orientation.x, m_ownerSpace->orientation.y, m_ownerSpace->orientation.z);
		glm::vec3 realPosition(m_ownerSpace->position.x, m_ownerSpace->position.y, m_ownerSpace->position.z);

		glm::quat finalOrientation(hmdOrientation);
		glm::vec3 finalPosition(hmdPosition + realPosition);

		glm::mat4 rotationMatrix = glm::mat4_cast(finalOrientation);
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), finalPosition);
		
		m_view = glm::inverse(translationMatrix * rotationMatrix);

		glm::mat4 poseMatrix = createProjectionMatrixFromXrFovf(view.fov, nearPlane, farPlane);
		m_proj = poseMatrix;

		skyboxShader.setMat4("view", m_view);
		skyboxShader.setMat4("proj", m_proj);

		shaderProgram.setMat4("view", m_view);
		shaderProgram.setMat4("proj", m_proj);
		shaderProgram.setVec3("camPos", glm::vec3(m_ownerSpace->position.x, m_ownerSpace->position.y, m_ownerSpace->position.z));
	}

	std::string Camera::toString()
	{
		std::ostringstream oss;
		oss << Component::toString();
		return oss.str();
	}
}
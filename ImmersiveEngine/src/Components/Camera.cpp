#include "Camera.h"

namespace ImmersiveEngine::cbs
{
	Camera::Camera(Object* obj) :
		Component(obj), fov(60.0f), nearPlane(0.1f), farPlane(100.0f),
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
	void Camera::refreshViewProjection(Shader& shaderProgram, const float aspectRatio)
	{
		if (m_ownerSpace == nullptr)
		{
			MISORDERED_ERROR;
			return;
		}

		m_view = glm::mat4(1.0f);
		m_proj = glm::mat4(1.0f);

		glm::vec3 forward(0, 0, -1);

		glm::vec3 pos(m_ownerSpace->position.x, m_ownerSpace->position.y, m_ownerSpace->position.z);
		glm::quat quat(m_ownerSpace->orientation.w, m_ownerSpace->orientation.x, m_ownerSpace->orientation.y, m_ownerSpace->orientation.z);
		glm::vec3 rot = quat * forward;
		glm::vec3 up(m_ownerSpace->up.x, m_ownerSpace->up.y, m_ownerSpace->up.z);
		m_view = glm::lookAt(pos, pos + rot, up); // Position of the world.
		m_proj = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);

		shaderProgram.setMat4("view", m_view);
		shaderProgram.setMat4("proj", m_proj);
		shaderProgram.setVec3("camPos", glm::vec3(m_ownerSpace->position.x, m_ownerSpace->position.y, m_ownerSpace->position.z));
	}

	glm::mat4 createProjectionMatrixFromXrFovf(const XrFovf& fov, float nearZ, float farZ)
	{
		// Tangent of the half-angles from the center axis
		float tanAngleLeft = tanf(fov.angleLeft);
		float tanAngleRight = tanf(fov.angleRight);
		float tanAngleDown = tanf(fov.angleDown);
		float tanAngleUp = tanf(fov.angleUp);

		float tanWidth = tanAngleRight - tanAngleLeft;
		float tanHeight = tanAngleUp - tanAngleDown;

		glm::mat4 proj;
		proj[0][0] = 2.0f / tanWidth;
		proj[0][1] = 0.0f;
		proj[0][2] = (tanAngleRight + tanAngleLeft) / tanWidth;
		proj[0][3] = 0.0f;

		proj[1][0] = 0.0f;
		proj[1][1] = 2.0f / tanHeight;
		proj[1][2] = (tanAngleUp + tanAngleDown) / tanHeight;
		proj[1][3] = 0.0f;

		proj[2][0] = 0.0f;
		proj[2][1] = 0.0f;
		proj[2][2] = -(farZ + nearZ) / (farZ - nearZ);
		proj[2][3] = -2.0f * farZ * nearZ / (farZ - nearZ);

		proj[3][0] = 0.0f;
		proj[3][1] = 0.0f;
		proj[3][2] = -1.0f;
		proj[3][3] = 0.0f;

		return proj;
	}

	/// Update the view and projection matrices of the camera (XR).
	void Camera::refreshViewProjection(Shader& shaderProgram, XrViewConfigurationView viewConfig, XrView view)
	{
		if (m_ownerSpace == nullptr)
		{
			MISORDERED_ERROR;
			return;
		}

		m_view = glm::mat4(1.0f);
		m_proj = glm::mat4(1.0f);

		float aspectRatio = viewConfig.recommendedImageRectWidth / viewConfig.recommendedImageRectHeight;
		glm::vec3 forward(0, 0, -1);

		glm::vec3 worldPosition(m_ownerSpace->position.x, m_ownerSpace->position.y, m_ownerSpace->position.z);
		glm::vec3 hmdPosition(view.pose.position.x, view.pose.position.y, view.pose.position.z);

		glm::quat worldOrientation(m_ownerSpace->orientation.w, m_ownerSpace->orientation.x, m_ownerSpace->orientation.y, m_ownerSpace->orientation.z);
		glm::quat hmdOrientation((float)view.pose.orientation.w, (float)view.pose.orientation.x, (float)view.pose.orientation.y, (float)view.pose.orientation.z);

		glm::vec3 realPosition = worldPosition + (worldOrientation * hmdPosition);
		glm::quat realOrientation = worldOrientation * hmdOrientation;

		glm::vec3 rot = realOrientation * forward;
		glm::vec3 up(m_ownerSpace->up.x, m_ownerSpace->up.y, m_ownerSpace->up.z);

		glm::mat4 eyeTransform = glm::translate(glm::mat4(1.0f), realPosition) * glm::mat4(realOrientation);
		m_view = glm::inverse(eyeTransform);
		m_proj = createProjectionMatrixFromXrFovf(view.fov, nearPlane, farPlane);

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
#include "Camera.h"

Camera::Camera() :
	fov(60.0f), nearPlane(0.1f), farPlane(100.0f),
	view(Engine::Math::Vector3(0, 0, -2.0f)),
	m_view(glm::mat4(1.0f)), m_proj(glm::mat4(1.0f)) { }

Camera::~Camera() { }

/// Update the view and projection matrices of the camera.
void Camera::updateViewProjection(Shader& shaderProgram, const float aspectRatio)
{
	m_view = glm::mat4(1.0f);
	m_proj = glm::mat4(1.0f);

	m_view = glm::translate(m_view, glm::vec3(view.x, view.y, view.z)); // Position of the world.
	m_proj = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);

	shaderProgram.setMat4("view", m_view);
	shaderProgram.setMat4("proj", m_proj);
}
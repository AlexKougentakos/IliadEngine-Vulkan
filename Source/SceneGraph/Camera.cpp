#include "Camera.h"

#include "glm/ext/matrix_clip_space.hpp"

namespace ili
{
	void Camera::SetOrthographicProjection(float left, float right, float bottom, float top, float near, float far)
	{
		m_ProjectionMatrix = glm::mat4{ 1.f };
		m_ProjectionMatrix[0][0] = 2.f / (right - left);
		m_ProjectionMatrix[1][1] = 2.f / (top - bottom);
		m_ProjectionMatrix[2][2] = 1.f / (far - near);
		m_ProjectionMatrix[3][0] = -(right + left) / (right - left);
		m_ProjectionMatrix[3][1] = -(top + bottom) / (top - bottom);
		m_ProjectionMatrix[3][2] = -near / (far - near);
	}

	void Camera::SetPerspectiveProjection(float fov, float aspect, float near, float far)
	{
		assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.f); //Check if aspect is not zero (division by zero)

		const float tanHalfFov = glm::tan(fov / 2.f);
		m_ProjectionMatrix = glm::mat4{ 0.f };
		m_ProjectionMatrix[0][0] = 1.f / (aspect * tanHalfFov);
		m_ProjectionMatrix[1][1] = 1.f / tanHalfFov;
		m_ProjectionMatrix[2][2] = far / (far - near);
		m_ProjectionMatrix[2][3] = 1.f;
		m_ProjectionMatrix[3][2] = -(far * near) / (far - near);
	}

	void Camera::SetViewDirection(const glm::vec3& cameraPosition, const glm::vec3& cameraDirection, const glm::vec3& up)
	{
		assert(glm::length(cameraDirection) > std::numeric_limits<float>::epsilon() && "The direction needs to be a non-zero vector");

		//Create an orthonormal basis
		const glm::vec3 w{ glm::normalize(cameraDirection) };
		const glm::vec3 u{ glm::normalize(glm::cross(w, up)) };
		const glm::vec3 v{ glm::cross(w, u) };

		m_ViewMatrix= glm::mat4{ 1.f };
		m_ViewMatrix[0][0] = u.x;
		m_ViewMatrix[1][0] = u.y;
		m_ViewMatrix[2][0] = u.z;
		m_ViewMatrix[0][1] = v.x;
		m_ViewMatrix[1][1] = v.y;
		m_ViewMatrix[2][1] = v.z;
		m_ViewMatrix[0][2] = w.x;
		m_ViewMatrix[1][2] = w.y;
		m_ViewMatrix[2][2] = w.z;
		m_ViewMatrix[3][0] = -glm::dot(u, cameraPosition);
		m_ViewMatrix[3][1] = -glm::dot(v, cameraPosition);
		m_ViewMatrix[3][2] = -glm::dot(w, cameraPosition);

		m_InverseViewMatrix = glm::mat4{ 1.f };
		m_InverseViewMatrix[0][0] = u.x;
		m_InverseViewMatrix[0][1] = u.y;
		m_InverseViewMatrix[0][2] = u.z;
		m_InverseViewMatrix[1][0] = v.x;
		m_InverseViewMatrix[1][1] = v.y;
		m_InverseViewMatrix[1][2] = v.z;
		m_InverseViewMatrix[2][0] = w.x;
		m_InverseViewMatrix[2][1] = w.y;
		m_InverseViewMatrix[2][2] = w.z;
		m_InverseViewMatrix[3][0] = cameraPosition.x;
		m_InverseViewMatrix[3][1] = cameraPosition.y;
		m_InverseViewMatrix[3][2] = cameraPosition.z;
	}

	void Camera::SetViewTarget(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) 
	{
		SetViewDirection(position, target - position, up);
	}

	void Camera::SetViewYXZ(const glm::vec3& position, const glm::vec3& rotation) 
	{
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
		const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };

		const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
		m_ViewMatrix = glm::mat4{ 1.f };
		m_ViewMatrix[0][0] = u.x;
		m_ViewMatrix[1][0] = u.y;
		m_ViewMatrix[2][0] = u.z;
		m_ViewMatrix[0][1] = v.x;
		m_ViewMatrix[1][1] = v.y;
		m_ViewMatrix[2][1] = v.z;
		m_ViewMatrix[0][2] = w.x;
		m_ViewMatrix[1][2] = w.y;
		m_ViewMatrix[2][2] = w.z;
		m_ViewMatrix[3][0] = -glm::dot(u, position);
		m_ViewMatrix[3][1] = -glm::dot(v, position);
		m_ViewMatrix[3][2] = -glm::dot(w, position);

		m_InverseViewMatrix = glm::mat4{ 1.f };
		m_InverseViewMatrix[0][0] = u.x;
		m_InverseViewMatrix[0][1] = u.y;
		m_InverseViewMatrix[0][2] = u.z;
		m_InverseViewMatrix[1][0] = v.x;
		m_InverseViewMatrix[1][1] = v.y;
		m_InverseViewMatrix[1][2] = v.z;
		m_InverseViewMatrix[2][0] = w.x;
		m_InverseViewMatrix[2][1] = w.y;
		m_InverseViewMatrix[2][2] = w.z;
		m_InverseViewMatrix[3][0] = position.x;
		m_InverseViewMatrix[3][1] = position.y;
		m_InverseViewMatrix[3][2] = position.z;
	}
}
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
}

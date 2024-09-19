#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "glm/ext/matrix_transform.hpp"

namespace ili
{
	class Camera final
	{
	public:

		void SetOrthographicProjection(float left, float right, float bottom, float top, float near, float far);
		void SetPerspectiveProjection(float fov, float aspect, float near, float far);

		void SetViewDirection(const glm::vec3& cameraPosition, const glm::vec3& cameraDirection, const glm::vec3& upVector = { 0.f, -1.f, 0.f });
		void SetViewTarget(const glm::vec3& cameraPosition, const glm::vec3& targetPosition, const glm::vec3& upVector = { 0.f, -1.f, 0.f });
		void SetViewYXZ(const glm::vec3& cameraPosition, const glm::vec3& rotation);

		const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }
		const glm::mat4& GetView() const { return m_ViewMatrix; }

	private:
		glm::mat4 m_ProjectionMatrix{ 1.f };
		glm::mat4 m_ViewMatrix{ 1.f };
	};
}

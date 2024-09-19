#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace ili
{
	class Camera final
	{
	public:

		void SetOrthographicProjection(float left, float right, float bottom, float top, float near, float far);
		void SetPerspectiveProjection(float fov, float aspect, float near, float far);

		const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }

	private:
		glm::mat4 m_ProjectionMatrix{ 1.f };
	};
}

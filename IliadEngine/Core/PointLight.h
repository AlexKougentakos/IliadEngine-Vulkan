#pragma once
#include "SceneGraph/GameObject.h"

namespace ili
{

	class PointLightGameObject final : public GameObject
	{
	public:
		PointLightGameObject(unsigned int id, float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f, 1.f, 1.f));
		virtual ~PointLightGameObject() override = default;

		PointLightGameObject(const PointLightGameObject&) = delete;
		PointLightGameObject(PointLightGameObject&&) = delete;
		PointLightGameObject& operator=(const PointLightGameObject&) = delete;
		PointLightGameObject& operator=(PointLightGameObject&&) = delete;

		void SetIntensity(float intensity) { m_Intensity = intensity; }
		float GetIntensity() const { return m_Intensity; }

		void SetRadius(float radius) { m_Radius = radius; }
		float GetRadius() const { return m_Radius; }

		void SetColor(const glm::vec3& color) { m_Color = color; }
		glm::vec3 GetColor() const { return m_Color; }

	private:
		float m_Intensity{};
		float m_Radius{};
		glm::vec3 m_Color{};

	};
}
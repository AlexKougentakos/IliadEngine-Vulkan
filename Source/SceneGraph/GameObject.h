#pragma once

#include "../Graphics/Model.h"
#include "glm/ext/matrix_transform.hpp"

namespace ili
{

	struct TransformComponent
	{
		glm::vec3 position{};
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation{0.f};

		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// To read the rotations as Intrinsic (local) read left to right. To read as Extrinsic (global) read right to left
		// Optimization to not calculate the matrices entirely:
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 GetMatrix() const;
		glm::mat3 GetNormalMatrix() const;
	};

	struct PointLightComponent
	{
		float lightIntensity{ 1.f };
	};

	class GameObject
	{
	public:
		~GameObject() = default;

		// Enable move semantics
		GameObject(GameObject&&) noexcept = default;
		GameObject& operator=(GameObject&&) noexcept = default;

		// Delete copy semantics
		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;

		void SetModel(std::shared_ptr<Model> model) { m_Model = model; }
		std::shared_ptr<Model> GetModel() const { return m_Model; }

		void SetColor(const glm::vec3& color) { m_Color = color; }
		glm::vec3 GetColor() const { return m_Color; }
		
		TransformComponent& GetTransform() { return m_TransformComponent; }
		const TransformComponent& GetTransformConst() const { return m_TransformComponent; }

		const std::unique_ptr<PointLightComponent>& GetPointLightComponent() { return m_PointLightComponent; }
		static GameObject MakePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f, 1.f, 1.f));

		void Translate(const glm::vec3& translation) { m_TransformComponent.position += translation; }

		unsigned int GetId() const { return m_Id; }
	private:
		friend class Scene;
		GameObject(const unsigned int id) : m_Id(id) {}

		glm::vec3 m_Color{};
		TransformComponent m_TransformComponent{};

		std::shared_ptr<Model> m_Model{};
		std::unique_ptr<PointLightComponent> m_PointLightComponent{};

		unsigned int m_Id{};
	};
}

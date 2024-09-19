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

		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// To read the rotations as Intrinsic (local) read left to right. To read as Extrinsic (global) read right to left
		// Optimization to not calculate the matrices entirely:
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4() {
			const float c3 = glm::cos(rotation.z);
			const float s3 = glm::sin(rotation.z);
			const float c2 = glm::cos(rotation.x);
			const float s2 = glm::sin(rotation.x);
			const float c1 = glm::cos(rotation.y);
			const float s1 = glm::sin(rotation.y);
			return glm::mat4{
			{
				scale.x * (c1 * c3 + s1 * s2 * s3),
				scale.x * (c2 * s3),
				scale.x * (c1 * s2 * s3 - c3 * s1),
				0.0f,
			},
			{
				scale.y * (c3 * s1 * s2 - c1 * s3),
				scale.y * (c2 * c3),
				scale.y * (c1 * c3 * s2 + s1 * s3),
				0.0f,
			},
			{
				scale.z * (c2 * s1),
				scale.z * (-s2),
				scale.z * (c1 * c2),
				0.0f,
			},
			{position.x, position.y, position.z, 1.0f} };
		}
	};

	class GameObject
	{
	public:

		~GameObject() = default;

		GameObject(const GameObject&) = default;
		GameObject(GameObject&&) = default;
		GameObject& operator=(const GameObject&) = delete;
		GameObject& operator=(GameObject&&) = delete;


		//Todo:: move this to the Scene when I add it
		static GameObject Create() 
		{ 
			static unsigned int currentId = 0;
			return GameObject(currentId++);
		}

		void SetModel(std::shared_ptr<Model> model) { m_Model = model; }
		std::shared_ptr<Model> GetModel() const { return m_Model; }

		void SetColor(const glm::vec3& color) { m_Color = color; }
		glm::vec3 GetColor() const { return m_Color; }
		
		TransformComponent& GetTransform() { return m_TransformComponent; }
		const TransformComponent& GetTransformConst() const { return m_TransformComponent; }


		void Translate(const glm::vec3& translation) { m_TransformComponent.position += translation; }

		unsigned int GetId() const { return m_Id; }
	private:
		GameObject(const unsigned int id) : m_Id(id) {}

		std::shared_ptr<Model> m_Model{};
		glm::vec3 m_Color{};
		TransformComponent m_TransformComponent{};

		unsigned int m_Id{};
	};
}

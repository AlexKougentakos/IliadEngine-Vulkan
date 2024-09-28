#pragma once
#include "BaseComponent.h"
#include <glm/glm.hpp>


namespace ili
{
	class TransformComponent : public BaseComponent
	{
	public:
		TransformComponent() = default;
		virtual ~TransformComponent() override = default;

		TransformComponent(const TransformComponent&) = delete;
		TransformComponent& operator=(const TransformComponent&) = delete;
		TransformComponent(TransformComponent&&) noexcept = delete;
		TransformComponent& operator=(TransformComponent&&) noexcept = delete;

		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// To read the rotations as Intrinsic (local) read left to right. To read as Extrinsic (global) read right to left
		// Optimization to not calculate the matrices entirely:
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 GetMatrix() const;
		glm::mat3 GetNormalMatrix() const;

		//todo: make these private at some point
		glm::vec3 position{};
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation{ 0.f };

	protected:
		void Initialize() override;
	};

}

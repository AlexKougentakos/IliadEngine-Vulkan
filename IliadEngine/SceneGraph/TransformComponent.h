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

        // Delete copy and move constructors and assignment operators
        TransformComponent(const TransformComponent&) = delete;
        TransformComponent& operator=(const TransformComponent&) = delete;
        TransformComponent(TransformComponent&&) noexcept = delete;
        TransformComponent& operator=(TransformComponent&&) noexcept = delete;

        // Getters
        glm::vec3 GetPosition() const;
        glm::vec3 GetScale() const;
        glm::vec3 GetRotationDegrees() const;
		glm::vec3 GetRotationRadians() const { return m_RotationRadians; }

        // Setters
        void SetPosition(const glm::vec3& newPosition);
        void SetScale(const glm::vec3& newScale);
        void SetRotationDegrees(const glm::vec3& newRotationDegrees);
		void SetRotationRadians(const glm::vec3& newRotationRadians) { m_RotationRadians = newRotationRadians; }

        // Transformation Methods
        void Translate(const glm::vec3& delta);
        void ScaleBy(const glm::vec3& scaleFactor);
        void RotateDegrees(const glm::vec3& deltaDegrees);

        // Matrix Calculations
        glm::mat4 GetMatrix() const;
        glm::mat3 GetNormalMatrix() const;

    protected:
        void Initialize() override;

    private:
        glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };
        glm::vec3 m_Scale{ 1.0f, 1.0f, 1.0f };
        glm::vec3 m_RotationRadians{ 0.0f, 0.0f, 0.0f }; // Stored internally in radians
    };
}

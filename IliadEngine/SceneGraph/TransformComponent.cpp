#include "TransformComponent.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ili
{
    void TransformComponent::Initialize()
    {
        // Initialization logic if needed
    }

    // Getters
    glm::vec3 TransformComponent::GetPosition() const
    {
        return m_Position;
    }

    glm::vec3 TransformComponent::GetScale() const
    {
        return m_Scale;
    }

    glm::vec3 TransformComponent::GetRotationDegrees() const
    {
        return glm::degrees(m_RotationRadians);
    }

    // Setters
    void TransformComponent::SetPosition(const glm::vec3& newPosition)
    {
        m_Position = newPosition;
    }

    void TransformComponent::SetScale(const glm::vec3& newScale)
    {
        m_Scale = newScale;
    }

    void TransformComponent::SetRotationDegrees(const glm::vec3& newRotationDegrees)
    {
        m_RotationRadians = glm::radians(newRotationDegrees);
    }

    // Transformation Methods
    void TransformComponent::Translate(const glm::vec3& delta)
    {
        m_Position += delta;
    }

    void TransformComponent::ScaleBy(const glm::vec3& scaleFactor)
    {
        m_Scale *= scaleFactor;
    }

    void TransformComponent::RotateDegrees(const glm::vec3& deltaDegrees)
    {
        m_RotationRadians += glm::radians(deltaDegrees);
    }

    // Matrix Calculations
    glm::mat4 TransformComponent::GetMatrix() const
    {
        // Calculate individual rotation matrices
        const float c1 = glm::cos(m_RotationRadians.y);
        const float s1 = glm::sin(m_RotationRadians.y);
        const float c2 = glm::cos(m_RotationRadians.x);
        const float s2 = glm::sin(m_RotationRadians.x);
        const float c3 = glm::cos(m_RotationRadians.z);
        const float s3 = glm::sin(m_RotationRadians.z);

        // Construct the rotation matrix using Tait-Bryan angles (Y-X-Z order)
        const glm::mat4 rotationMatrix = glm::mat4(
            glm::vec4(c1 * c3 + s1 * s2 * s3, c2 * s3, c1 * s2 * s3 - c3 * s1, 0.0f),
            glm::vec4(c3 * s1 * s2 - c1 * s3, c2 * c3, c1 * c3 * s2 + s1 * s3, 0.0f),
            glm::vec4(c2 * s1, -s2, c1 * c2, 0.0f),
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
        );

        // Apply scaling
        const glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), m_Scale);

        // Apply translation
        const glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), m_Position);

        // Combine transformations: Translation * Rotation * Scale
        return translationMatrix * rotationMatrix * scalingMatrix;
    }

    glm::mat3 TransformComponent::GetNormalMatrix() const
    {
        // The normal matrix is the transpose of the inverse of the upper-left 3x3 of the model matrix
        const glm::mat4 modelMatrix = GetMatrix();
        const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
        return normalMatrix;
    }
}

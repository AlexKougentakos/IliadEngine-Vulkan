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
        // Start with an identity matrix
        glm::mat4 modelMatrix = glm::mat4(1.0f);

        // Apply translation
        modelMatrix = glm::translate(modelMatrix, m_Position);

        // Apply rotation in Y-X-Z order
        modelMatrix = glm::rotate(modelMatrix, m_RotationRadians.y, glm::vec3(0.0f, 1.0f, 0.0f)); // Y-axis
        modelMatrix = glm::rotate(modelMatrix, m_RotationRadians.x, glm::vec3(1.0f, 0.0f, 0.0f)); // X-axis
        modelMatrix = glm::rotate(modelMatrix, m_RotationRadians.z, glm::vec3(0.0f, 0.0f, 1.0f)); // Z-axis

        // Apply scaling
        modelMatrix = glm::scale(modelMatrix, m_Scale);

        return modelMatrix;
    }


    glm::mat3 TransformComponent::GetNormalMatrix() const
    {
        // The normal matrix is the transpose of the inverse of the upper-left 3x3 of the model matrix
        const glm::mat4 modelMatrix = GetMatrix();
        const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
        return normalMatrix;
    }
}

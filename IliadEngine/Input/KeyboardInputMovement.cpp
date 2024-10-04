#include "KeyboardInputMovement.h"
#include <glm/gtc/constants.hpp>
#include <limits>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp> // For rotation transformations

namespace ili {

	void KeyboardMovementController::MoveInPlaneXZ(GLFWwindow* window, const float deltaTime, GameObject& gameObject)
	{
		glm::vec3 rotation{};

		// Handle Keyboard Rotation Inputs
		if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotation.y -= 1;
		if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotation.y += 1;
		if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotation.x += 1;
		if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotation.x -= 1;

		// Handle Mouse Rotation Inputs
		double currentMouseX, currentMouseY;
		glfwGetCursorPos(window, &currentMouseX, &currentMouseY);

		if (firstMouse)
		{
			lastMouseX = currentMouseX;
			lastMouseY = currentMouseY;
			firstMouse = false;
		}

		double deltaMouseX = currentMouseX - lastMouseX;
		double deltaMouseY = currentMouseY - lastMouseY;

		lastMouseX = currentMouseX;
		lastMouseY = currentMouseY;

		// **Inverted Y-axis Mouse Movement**
		rotation.y += static_cast<float>(deltaMouseX) * mouseSensitivity;
		rotation.x -= static_cast<float>(deltaMouseY) * mouseSensitivity; // Invert Y-axis

		// Accumulate Rotation with deltaTime Scaling
		accumulatedRotation.x += rotation.x * deltaTime;
		accumulatedRotation.x = glm::clamp(accumulatedRotation.x, -1.5f, 1.5f); // Clamp Pitch to prevent flipping

		accumulatedRotation.y += rotation.y * deltaTime;
		accumulatedRotation.y = glm::mod(accumulatedRotation.y, glm::two_pi<float>()); // Wrap Yaw within [0, 2π)

		// Update GameObject's Rotation
		gameObject.GetTransform()->SetRotationRadians(accumulatedRotation);

		// **Calculate Directional Vectors Based on Updated Pitch and Yaw**
		const float pitch = accumulatedRotation.x;
		const float yaw = accumulatedRotation.y;

		// Calculate forward vector including pitch
		glm::vec3 forward;
		forward.x = cos(pitch) * sin(yaw);
		forward.y = -sin(pitch); // Inverted Y-component
		forward.z = cos(pitch) * cos(yaw);
		forward = glm::normalize(forward);

		// Calculate right vector as cross product of forward and world up
		glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

		// Recalculate up vector as cross product of right and forward
		glm::vec3 up = glm::normalize(glm::cross(right, forward));

		// Handle Movement Inputs Relative to Camera's Orientation
		glm::vec3 moveDirection{};

		if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDirection += forward;
		if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDirection -= forward;
		if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDirection -= right;
		if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDirection += right;
		if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDirection += up;
		if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDirection -= up;

		if (glm::length(moveDirection) > std::numeric_limits<float>::epsilon())
		{
			glm::vec3 normalizedMoveDirection = glm::normalize(moveDirection);
			glm::vec3 movementDelta = movementSpeed * normalizedMoveDirection * deltaTime;
			gameObject.GetTransform()->SetPosition(gameObject.GetTransform()->GetPosition() + movementDelta);
		}
	}


} // namespace ili

#include "KeyboardInputMovement.h"

#include "SceneGraph/TransformComponent.h"

void ili::KeyboardMovementController::MoveInPlaneXZ(GLFWwindow* window, const float deltaTime, GameObject& gameObject)
{
	glm::vec3 rotation{};
	if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotation.y -= 1;
	if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotation.y += 1;
	if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotation.x += 1;
	if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotation.x -= 1;

	// Make sure that the rotation is not zero, because if you normalize a zero vector, it will result in a NaN
	if (glm::dot(rotation, rotation) > std::numeric_limits<float>::epsilon())
		gameObject.GetTransform()->rotation += turnSpeed * glm::normalize(rotation) * deltaTime;

	gameObject.GetTransform()->rotation.x = glm::clamp(gameObject.GetTransform()->rotation.x, -1.5f, 1.5f);
	gameObject.GetTransform()->rotation.y = glm::mod(gameObject.GetTransform()->rotation.y, glm::two_pi<float>());

	const float yaw = gameObject.GetTransform()->rotation.y;
	const glm::vec3 forward{ sin(yaw), 0, cos(yaw) };
	const glm::vec3 right{ forward.z, 0, -forward.x };
	constexpr glm::vec3 up{ 0, -1.f, 0 };

	glm::vec3 moveDirection{};
	if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDirection += forward;
	if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDirection -= forward;
	if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDirection += right;
	if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDirection -= right;
	if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDirection += up;
	if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDirection -= up;

	if (glm::dot(moveDirection, moveDirection) > std::numeric_limits<float>::epsilon())
		gameObject.GetTransform()->position += movementSpeed * glm::normalize(moveDirection) * deltaTime;
}

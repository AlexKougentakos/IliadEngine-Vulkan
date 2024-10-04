#pragma once

#include "SceneGraph/GameObject.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace ili
{
    class KeyboardMovementController final
    {
    public:
        struct KeyMappings
        {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_E;
            int moveDown = GLFW_KEY_Q;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
        };

        void MoveInPlaneXZ(GLFWwindow* window, const float deltaTime, GameObject& gameObject);

        KeyMappings keys{};
        float movementSpeed = 3.0f;
        float turnSpeed = 1.5f;

    private:
        // Mouse handling member variables
        bool firstMouse = true;
        double lastMouseX = 0.0;
        double lastMouseY = 0.0;
        float mouseSensitivity = 0.1f;
        glm::vec3 accumulatedRotation = glm::vec3(0.0f);
    };
}

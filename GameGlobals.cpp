//
// Created by Garret on 1/11/2023.
//

#include "GameGlobals.h"
#include "GLFW/glfw3.h"

namespace GameGlobals {
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    void updateDeltaTime() {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
    }
}
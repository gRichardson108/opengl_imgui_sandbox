//
// Created by Garret on 1/8/2023.
//
#include "MainCamera.h"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"

namespace MainCamera {
    int screen_width;
    int screen_height;
    glm::vec3 camera_position = glm::vec3(0.f, 0.f, 3.f);
    glm::vec3 camera_front = glm::vec3(0.f, 0.f, -1.f);
    glm::vec3 camera_up = glm::vec3(0.f, 1.f, 0.f);

    bool first_mouse = true;
    MouseInfo mouse_info = {
            .mouse_look = false,
            .lastX = 0.0,
            .lastY = 0.0,
            .pitch = 0.f,
            .yaw = -90.f
    };

    void handleMouseMovement(double xpos, double ypos) {
        if (!mouse_info.mouse_look) return;

        if (first_mouse) {
            mouse_info.lastX = xpos;
            mouse_info.lastY = ypos;
            first_mouse = false;
        }

        float xoffset = xpos - mouse_info.lastX;
        float yoffset = mouse_info.lastY - ypos;
        mouse_info.lastX = xpos;
        mouse_info.lastY = ypos;

        // todo: expose mouse sensitivity as menu option
        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        mouse_info.yaw += xoffset;
        mouse_info.pitch += yoffset;

        if (mouse_info.pitch > 89.f) {
            mouse_info.pitch = 89.f;
        }
        if (mouse_info.pitch < -89.f) {
            mouse_info.pitch = -89.f;
        }

        glm::vec3 direction;
        direction.x = cos(glm::radians(mouse_info.yaw)) * cos(glm::radians(mouse_info.pitch));
        direction.y = sin(glm::radians(mouse_info.pitch));
        direction.z = sin(glm::radians(mouse_info.yaw)) * cos(glm::radians(mouse_info.pitch));
        MainCamera::camera_front = glm::normalize(direction);
    }
}

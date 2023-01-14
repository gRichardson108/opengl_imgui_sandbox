//
// Created by Garret on 1/8/2023.
//
#include "MainCamera.h"

namespace MainCamera {
    int screen_width;
    int screen_height;
    glm::vec3 camera_position = glm::vec3(0.f, 0.f, 3.f);
    glm::vec3 camera_front = glm::vec3(0.f, 0.f, -1.f);
    glm::vec3 camera_up = glm::vec3(0.f, 1.f, 0.f);
    bool mouse_look = false;
}

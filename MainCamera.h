//
// Created by Garret on 1/8/2023.
//

#ifndef HEATMAP_MAINCAMERA_H
#define HEATMAP_MAINCAMERA_H

#include "glm/vec3.hpp"

namespace MainCamera {
    extern int screen_width;
    extern int screen_height;
    extern glm::vec3 camera_position;
    extern glm::vec3 camera_front;
    extern glm::vec3 camera_up;

    extern bool first_mouse;
    struct MouseInfo {
        bool mouse_look;
        double lastX;
        double lastY;
        float pitch;
        float yaw;
    };
    extern MouseInfo mouse_info;

    void handleMouseMovement(double xpos, double ypos);
}

#endif //HEATMAP_MAINCAMERA_H

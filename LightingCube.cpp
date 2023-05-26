//
// Created by Garret on 2/12/2023.
//

#include "LightingCube.h"

LightingCube::LightingCube() :
        vao(0),
        vbo(0),
        ebo(0),
        shader("resources/light_cube.vsh.glsl",
               "resources/light_cube.fsh.glsl") {

}

LightingCube::~LightingCube() {

}

void LightingCube::draw() {

}

void LightingCube::update() {

}

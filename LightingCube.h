//
// Created by Garret on 2/12/2023.
//

#ifndef HEATMAP_LIGHTINGCUBE_H
#define HEATMAP_LIGHTINGCUBE_H

#include "Shader.h"
#include "glad/gl.h"

/// A cube that acts as a light source.
class LightingCube {
public:
    LightingCube();

    ~LightingCube();

    void draw();

    void update();

private:
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    Shader shader;
    GLint modelId;
    GLint viewId;
    GLint projectionId;
};


#endif //HEATMAP_LIGHTINGCUBE_H

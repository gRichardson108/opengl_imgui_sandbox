//
// Created by Garret on 1/8/2023.
//

#ifndef HEATMAP_BOIDCLOUD_H
#define HEATMAP_BOIDCLOUD_H

#include "Shader.h"
#include "glad/gl.h"

/// Draws a cloud of boid-like triangles according to the parameters given.
class BoidCloud {
public:
    BoidCloud();
    ~BoidCloud();
    
    struct BoidParams {
        int quantity;
        float scale;
    };
    
    void setBoidParameters(const BoidParams& params);
    void draw();
private:
    BoidParams params;
    unsigned int textureId;
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    Shader shader;
    GLint modelId;
    GLint viewId;
    GLint projectionId;
};


#endif //HEATMAP_BOIDCLOUD_H

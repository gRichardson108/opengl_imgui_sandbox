//
// Created by Garret on 1/8/2023.
//

#ifndef HEATMAP_BOIDCLOUD_H
#define HEATMAP_BOIDCLOUD_H

#include <vector>
#include "Shader.h"
#include "glad/gl.h"
#include "glm/vec3.hpp"

/// Draws a cloud of boid-like triangles according to the parameters given.
class BoidCloud {
public:
    BoidCloud();

    ~BoidCloud();

    struct BoidParams {
        int quantity;
        float scale;
        glm::vec3 boundingVolume;
        float minimumSpeed;
        float maximumSpeed;
    };

    void setBoidParameters(const BoidParams &params);

    void draw();

    void update();

private:
    struct Boid {
        glm::vec3 position;
        glm::vec3 direction;
        float speed;
    };
    std::vector<Boid> boids;
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

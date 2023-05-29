//
// Created by Garret on 12/29/2022.
//

#ifndef HEATMAP_HEATMAP_H
#define HEATMAP_HEATMAP_H

#include "Shader.h"

class Heatmap {
public:
    Heatmap(int width, int height);

    ~Heatmap();

    void fillWithNoise();

    void draw();

private:
    void updateTexture();

    /// GL_RGB8 data
    void *data;
    int width;
    int height;
    unsigned int textureId;
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    Shader shader;
};


#endif //HEATMAP_HEATMAP_H

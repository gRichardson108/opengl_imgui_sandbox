//
// Created by Garret on 12/29/2022.
//

#include <cmath>
#include "Heatmap.h"
#include <glad/gl.h>
#include <glm/vec2.hpp>

#define GL_CALL(_CALL)      do { _CALL; GLenum gl_err = glGetError(); if (gl_err != 0) fprintf(stderr, "%s:%d GL error 0x%x returned from '%s'.\n", __FILE__, __LINE__, gl_err, #_CALL); } while (0)  // Call with error check
#define GL_CHECK      do { GLenum gl_err = glGetError(); if (gl_err != 0) fprintf(stderr, "%s:%d GL error 0x%x returned \n", __FILE__, __LINE__, gl_err); } while (0)  // Just error check

struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

const Color WHITE = {255, 255, 255, 255};
const Color BLACK = {0, 0, 0, 255};

Heatmap::Heatmap(int width, int height) :
        width(width),
        height(height),
        textureId(0),
        vao(0),
        vbo(0),
        ebo(0),
        shader("resources/texture.vsh", "resources/texture.fsh") {
    data = malloc(sizeof(Color) * width * height);
    memset(data, 0, sizeof(Color) * width * height);

    // initialize the VBO, VAO
    float vertices[] = {
            // positions          // colors           // texture coords
            0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
            0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
            -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
    };
    GL_CHECK;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    GL_CHECK;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    GL_CHECK;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    GL_CHECK;

    // position attribute
    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0));
    GL_CALL(glEnableVertexAttribArray(0));
    // color attribute
    GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float))));
    GL_CALL(glEnableVertexAttribArray(1));
    // texture coord attribute
    GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float))));
    GL_CALL(glEnableVertexAttribArray(2));

    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GL_CHECK;

    // allocate an OpenGL texture
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, (unsigned char *) data);

    GL_CHECK;
    // configure initial shader params
    shader.use();
    GL_CHECK;
    glUniform1i(glGetUniformLocation(shader.id, "texture1"), 0);
    auto textureOffsetId = glGetUniformLocation(shader.id, "textureOffset");
    auto textureScaleId = glGetUniformLocation(shader.id, "textureScale");
    GLfloat offset[2] = {};
    GLfloat scale[2] = {1.0f, 1.0f};
    glUniform2fv(textureOffsetId, 1, offset);
    glUniform2fv(textureScaleId, 1, scale);
}

Heatmap::~Heatmap() {
    free(data);
    glDeleteTextures(1, &textureId);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

/// Helper function that generates a random integer between bounds.
/// \param min Minimum value, inclusive.
/// \param max Maximum value, inclusive.
/// \return Random integer
int getRandomValue(int min, int max) {
    if (min > max) {
        int tmp = max;
        max = min;
        min = tmp;
    }

    return (rand() % (abs(max - min) + 1) + min);
}

void Heatmap::fillWithNoise() {
    const float FACTOR = 0.5f;
    Color *pixels = (Color *) data;
    for (int i = 0; i < width * height; i++) {
        if (getRandomValue(0, 99) < (int) (FACTOR * 100.0f)) pixels[i] = WHITE;
        else pixels[i] = BLACK;
    }
    updateTexture();
}

void Heatmap::updateTexture() {
    GL_CHECK;
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, (unsigned char *) data);

    GL_CHECK;
    // configure initial shader params
    shader.use();
    GL_CHECK;
    glUniform1i(glGetUniformLocation(shader.id, "texture1"), 0);
    auto textureOffsetId = glGetUniformLocation(shader.id, "textureOffset");
    auto textureScaleId = glGetUniformLocation(shader.id, "textureScale");
    GLfloat offset[2] = {};
    GLfloat scale[2] = {1.0f, 1.0f};
    glUniform2fv(textureOffsetId, 1, offset);
    glUniform2fv(textureScaleId, 1, scale);

#if 0
    GL_CALL(glActiveTexture(GL_TEXTURE0));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, textureId));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
                         0, GL_RGB, GL_UNSIGNED_BYTE, (unsigned char *) data));
    GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
#endif
}

void Heatmap::draw() {
    GL_CALL(glActiveTexture(GL_TEXTURE0));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, textureId));
    shader.use();
    GL_CALL(glBindVertexArray(vao));
    GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
}

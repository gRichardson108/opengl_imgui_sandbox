//
// Created by Garret on 1/8/2023.
//

#include <iostream>
#include <random>

#include "BoidCloud.h"
#include "MainCamera.h"
#include "GLFW/glfw3.h"
#include "stb_image.h"
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GL_CHECK      do { GLenum gl_err = glGetError(); if (gl_err != 0) fprintf(stderr, "%s:%d GL error 0x%x returned \n", __FILE__, __LINE__, gl_err); } while (0)  // Just error check

std::random_device rd;
std::mt19937 random_generator(rd());

BoidCloud::BoidCloud() :
        shader("resources/texturedMesh.vsh.glsl", "resources/texturedMesh.fsh.glsl") {
    // initialize the VBO, VAO
#if 0
    float vertices[] = {
            // positions                     // colors                        // texture coords
            0.0f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
            0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
           -0.5f,  -0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // top left 
           -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // bottom left 
    };
    unsigned int indices[] = {  // note that we start from 0!
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
    };
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

#endif
    float vertices[] = {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
    };
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // load texture from file
    int width, height, nrChannels;
    unsigned char *data = stbi_load("resources/trout.jpg", &width, &height, &nrChannels, 0);

    // allocate an OpenGL texture
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    GL_CHECK;
    std::cerr << "Checking texture bind calls" << std::endl;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GL_CHECK;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    // configure initial shader params
    shader.use();

    modelId = glGetUniformLocation(shader.id, "model");
    viewId = glGetUniformLocation(shader.id, "view");
    projectionId = glGetUniformLocation(shader.id, "projection");

    glUniform1i(glGetUniformLocation(shader.id, "texture1"), 0);
}

BoidCloud::~BoidCloud() {
    glDeleteTextures(1, &textureId);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void BoidCloud::setBoidParameters(const BoidCloud::BoidParams &params) {
    this->params = params;
    while (params.quantity > boids.size()) {
        std::uniform_real_distribution<float> distribution_x(0.0, params.boundingVolume.x);
        std::uniform_real_distribution<float> distribution_y(0.0, params.boundingVolume.y);
        std::uniform_real_distribution<float> distribution_z(0.0, params.boundingVolume.z);
        boids.push_back(Boid{
                .position = glm::vec3(
                    distribution_x(random_generator),
                    distribution_y(random_generator),
                    distribution_z(random_generator)),
                .direction = glm::vec3(1.f, 0.f, 0.0),
                .speed = 0.05f
        });
    }
    while (boids.size() > params.quantity) {
        boids.pop_back();
    }
}

void BoidCloud::draw() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // set the scale
    shader.use();

    const float max_distance = 4.0f;
    for (unsigned int i = 0; i < boids.size(); i++) {
        // draw the boids wherever they are
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 view = glm::lookAt(MainCamera::camera_position,
                                     MainCamera::camera_position + MainCamera::camera_front,
                                     MainCamera::camera_up);
        glm::mat4 projection = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.5f));
        model = glm::translate(model, boids[i].position);
        // scale it
        model = glm::scale(model, glm::vec3(params.scale));

        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        projection = glm::perspective(glm::radians(45.0f),
                                      (float) MainCamera::screen_width / (float) MainCamera::screen_height, 0.1f,
                                      100.0f);
        glUniformMatrix4fv(modelId, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewId, 1, GL_FALSE, glm::value_ptr(view));
        // preferably this would just happen in the camera class somewhere, since it is the same for almost everything
        glUniformMatrix4fv(projectionId, 1, GL_FALSE, glm::value_ptr(projection));

        // draw the boids
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void BoidCloud::update() {
    for (Boid &boid: boids) {
        boid.position += boid.direction * boid.speed;
        if (boid.position.x > params.boundingVolume.x) {
            boid.position.x = 0;
        }
        if (boid.position.y > params.boundingVolume.y) {
            boid.position.y = 0;
        }
        if (boid.position.z > params.boundingVolume.z) {
            boid.position.z = 0;
        }
    }
}

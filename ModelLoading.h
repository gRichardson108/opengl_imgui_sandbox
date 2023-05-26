//
// Created by Garret on 5/25/2023.
//

#ifndef HEATMAP_MODELLOADING_H
#define HEATMAP_MODELLOADING_H

#include <vector>

#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

#include "Shader.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
};

enum TextureType {
    Diffuse,
    Specular
};

struct Texture {
    unsigned int id;
    TextureType textureType;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
};

void setup(Mesh *mesh);

void draw(const Mesh *mesh, const Shader *shader);

struct Model {
    std::vector<Mesh> meshes;
    std::string directory;
};

Model loadModel(const std::string *path);

// forward declaration for assimp
struct aiNode;
struct aiScene;
struct aiMesh;

void processNode(Model *model, aiNode *node, const aiScene *scene);
Mesh processMesh(const aiMesh* mesh, const aiScene* scene);

void draw(const Model *model, const Shader *shader);

#endif //HEATMAP_MODELLOADING_H

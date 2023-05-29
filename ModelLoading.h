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
    std::string path;
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
    std::vector<Texture> textures_loaded;
};

Model loadModel(const std::string &path);

// forward declaration for assimp
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;

void processNode(Model *model, aiNode *node, const aiScene *scene);
Mesh processMesh(Model* model, const aiMesh* mesh, const aiScene* scene);

void draw(const Model *model, const Shader *shader);

std::vector<Texture> loadMaterialTextures(Model* model, aiMaterial *mat, aiTextureType type, TextureType textureType);

/// Uploads a texture using some default wrapping / mipmap behavior.
/// \param path Path to the filename, relative to the directory.
/// \param directory Root directory containing the file.
/// \param gamma Unused.
/// \return texture ID
unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma=false);

#endif //HEATMAP_MODELLOADING_H

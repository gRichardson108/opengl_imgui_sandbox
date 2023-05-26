//
// Created by Garret on 5/25/2023.
// Most of this is taken from Joey DeVries, at https://learnopengl.com/Model-Loading/Mesh
//

#include "ModelLoading.h"
#include "glad/gl.h"

#include <iostream>

void setup(Mesh *mesh) {
    glGenVertexArrays(1, &mesh->vao);
    glGenBuffers(1, &mesh->vbo);
    glGenBuffers(1, &mesh->ebo);

    glBindVertexArray(mesh->vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(Vertex), &mesh->vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), &mesh->indices[0],
                 GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tex_coords));

    glBindVertexArray(0);
}

void draw(const Mesh *mesh, const Shader *shader) {
    unsigned int diffuse_num = 1;
    unsigned int specular_num = 1;
    // TODO: All of the names should just get figured out when setup is called, and the index for each texture stored in the Mesh somewhere
    for (unsigned int i = 0; i < mesh->textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string number;
        std::string name;
        switch (mesh->textures[i].textureType) {
            case Specular:
                number = std::to_string(specular_num++);
                name = "texture_specular";
                break;
            case Diffuse:
                number = std::to_string(diffuse_num++);
                name = "texture_diffuse";
                break;
            default:
                assert(false);
                break;
        }
        shader->setInt("material." + name + number, i);
        glBindTexture(GL_TEXTURE_2D, mesh->textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    // draw mesh
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

void draw(const Model *model, const Shader *shader) {
    for (unsigned int i = 0; model->meshes.size(); i++) {
        draw(&model->meshes[i], shader);
    }
}

Model loadModel(const std::string* path) {
    Model output = {};
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path->c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return output;
    }
    // TODO: This is platform specific code for directories. Can it be removed later?
    output.directory = path->substr(0, path->find_last_of('/'));
    processNode(&output, scene->mRootNode, scene);
    
    return output;
}

void processNode(Model *model, aiNode *node, const aiScene *scene) {
    // process all the node's meshes
    for (unsigned int i=0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        model->meshes.push_back(processMesh(mesh, scene));
    }
    // repeat for any child nodes
    for (unsigned int i=0; i < node->mNumChildren; i++) {
        processNode(model, node->mChildren[i], scene);
    }
}

Mesh processMesh(const aiMesh *mesh, const aiScene *scene) {
    // TODO
    return Mesh();
}

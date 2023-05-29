//
// Created by Garret on 5/25/2023.
// Most of this is taken from Joey DeVries, at https://learnopengl.com/Model-Loading/Mesh
//

#include "ModelLoading.h"
#include "glad/gl.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stb_image.h"

#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.f, 0.f, 0.f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    shader->setMat4("model", model);
    
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
        shader->setInt((name + number), i);
        glBindTexture(GL_TEXTURE_2D, mesh->textures[i].id);
    }
    // draw mesh
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // reset
    glActiveTexture(GL_TEXTURE0);
}

void draw(const Model *model, const Shader *shader) {
    for (const Mesh& mesh : model->meshes) {
        draw(&mesh, shader);
    }
}

Model loadModel(const std::string &path) {
    Model output = {};
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return output;
    }
    // TODO: This is platform specific code for directories. Can it be removed later?
    output.directory = path.substr(0, path.find_last_of('/'));
    processNode(&output, scene->mRootNode, scene);

    return output;
}

void processNode(Model *model, aiNode *node, const aiScene *scene) {
    // process all the node's meshes
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        model->meshes.push_back(processMesh(model, mesh, scene));
    }
    // repeat for any child nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(model, node->mChildren[i], scene);
    }
}

Mesh processMesh(Model* model, const aiMesh *mesh, const aiScene *scene) {
    Mesh output = {};
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex = {};
        // process vertex, texture coords, normals
        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        if (mesh->mTextureCoords[0]) {
            vertex.tex_coords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        output.vertices.push_back(vertex);
    }
    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        const aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            output.indices.push_back(face.mIndices[j]);
        }
    }

    // process material
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(model, material, aiTextureType_DIFFUSE, TextureType::Diffuse);
        output.textures.insert(output.textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMaterialTextures(model, material, aiTextureType_SPECULAR, TextureType::Specular);
        output.textures.insert(output.textures.end(), specularMaps.begin(), specularMaps.end());
    }

    setup(&output);
    return output;
}

std::vector<Texture> loadMaterialTextures(Model* model, aiMaterial *mat, aiTextureType type, TextureType textureType) {
    std::vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (const Texture &tex : model->textures_loaded) {
            if (tex.path == std::string(str.C_Str())) {
                textures.push_back(tex);
                skip = true;
                break;
            }
        }
        
        if (!skip) {
            std::cerr << "Loading " << str.C_Str() << std::endl;
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), model->directory);
            texture.textureType = textureType;
            texture.path = str.C_Str();
            textures.push_back(texture);
            model->textures_loaded.push_back(texture);
        }
    }
    return textures;
}

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma) {
    std::string filename = std::string(path);
    filename = directory + '/' + filename;
    
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, numComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &numComponents, 0);
    if (data) {
        GLenum format;
        switch (numComponents) {
            case 1:
                format = GL_RED;
                break;
            case 2:
                format = GL_RG;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
            default:
                std::cerr << "Error: invalid number of components in file " << filename << std::endl;
                break;
        }
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    } else {
        std::cerr << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    
    return textureID;
}

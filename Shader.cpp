//
// Created by Garret on 12/28/2022.
//

#include "Shader.h"

#include <glad/gl.h>

#include <fstream>
#include <sstream>
#include <iostream>

#define GL_CHECK      do { GLenum gl_err = glGetError(); if (gl_err != 0) fprintf(stderr, "%s:%d GL error 0x%x returned \n", __FILE__, __LINE__, gl_err); } while (0)  // Just error check

Shader::Shader(const char *vertexPath, const char *fragmentPath) :
        id(0) {
    using namespace std;

    string vertexCode;
    string fragmentCode;
    ifstream vShaderFile;
    ifstream fShaderFile;

    // allows the ifstreams to throw exceptions
    vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
    fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

    // attempt to read the sources
    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (ifstream::failure &e) {
        cerr << "Error: Shader file not successfully read: " << e.what() << endl;
    }

    GL_CHECK;
    // compile sources
    unsigned int vertexId = 0;
    const char *vertexCodeSource = vertexCode.c_str();
    vertexId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexId, 1, &vertexCodeSource, NULL);
    glCompileShader(vertexId);
    GL_CHECK;
    checkCompileErrors(vertexId, ShaderType::Vertex);

    unsigned int fragmentId = 0;
    const char *fragmentCodeSource = fragmentCode.c_str();
    fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentId, 1, &fragmentCodeSource, NULL);
    glCompileShader(fragmentId);
    checkCompileErrors(fragmentId, ShaderType::Fragment);
    id = glCreateProgram();
    glAttachShader(id, vertexId);
    glAttachShader(id, fragmentId);
    glLinkProgram(id);
    GL_CHECK;
    checkCompileErrors(id, ShaderType::Program);
    GL_CHECK;
    // once the shaders are linked, we can delete the individual shaders
    GL_CHECK;
    glDeleteShader(vertexId);
    GL_CHECK;
    glDeleteShader(fragmentId);
    GL_CHECK;
}

void Shader::use() {
    glUseProgram(id);
    GL_CHECK;
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int) value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::checkCompileErrors(unsigned int shader, Shader::ShaderType shaderType) {
    int success;
    char infoLog[1024];
    if (shaderType != ShaderType::Program) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "Error: could not compile shader source: " << infoLog << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "Error: could not link shader program: " << infoLog << std::endl;
        }
    }
}

//
// Created by Garret on 12/28/2022.
//

#include "Shader.h"

#include <glad/gl.h>

#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char *vertexPath, const char *fragmentPath) : 
    id(0)
{
    using namespace std;
    
    string vertexCode;
    string fragmentCode;
    ifstream vShaderFile;
    ifstream fShaderFile;
    
    // allows the ifstreams to throw exceptions
    vShaderFile.exceptions(ifstream ::failbit | ifstream::badbit);
    fShaderFile.exceptions(ifstream ::failbit | ifstream::badbit);
    
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
    } catch (ifstream::failure& e) {
        cerr << "Error: Shader file not successfully read: " << e.what() << endl;
    }
    
    // compile sources
    unsigned int vertexId = 0;
    const char* vertexCodeSource = vertexCode.c_str();
    vertexId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexId, 1, &vertexCodeSource, NULL);
    
    
    unsigned int fragmentId = 0;
    fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
}

void Shader::use() {

}

void Shader::setBool(const std::string &name, bool value) const {

}

void Shader::setInt(const std::string &name, int value) const {

}

void Shader::setFloat(const std::string &name, float value) const {

}

void Shader::checkCompileErrors(unsigned int shader, Shader::ShaderType shaderType) {

}

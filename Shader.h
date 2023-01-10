//
// Created by Garret on 12/28/2022.
//

#ifndef HEATMAP_SHADER_H
#define HEATMAP_SHADER_H

#include <string>

/// Handles shader compilation in a very basic way, with error messages.
class Shader {
public:
/// The ID of the shader program after linking. Set to 0 for error.
    unsigned int id;
    
    /// Construct the shader. Prints error messages if there's a compilation error.
    /// \param vertexPath Path to the vertex shader source file.
    /// \param fragmentPath Path to the fragment shader source file.
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    /// Activates the shader for draw calls.
    void use();

    /// Sets a shader uniform for a boolean parameter.
    /// \param name Shader uniform parameter name.
    /// \param value Value to set.
    void setBool(const std::string &name, bool value) const;

    /// Sets a shader uniform for an integer parameter.
    /// \param name Shader uniform parameter name.
    /// \param value Value to set.
    void setInt(const std::string &name, int value) const;

    /// Sets a shader uniform for a float parameter.
    /// \param name Shader uniform parameter name.
    /// \param value Value to set.
    void setFloat(const std::string &name, float value) const;

private:
    enum class ShaderType {
        Vertex,
        Fragment,
        Program
    };
    
    std::string vertexPath;
    std::string fragmentPath;
    /// Performs a check for compile-time errors, printing error messages if needed.
    /// \param shader ID of the shader to check.
    /// \param type Type of the shader program.
    void checkCompileErrors(unsigned int shader, ShaderType shaderType);
};


#endif //HEATMAP_SHADER_H

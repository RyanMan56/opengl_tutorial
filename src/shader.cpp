#include "shader.h"

#include <glad/glad.h> // Include glad to get all of the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

void checkShaderCompilation(unsigned int &shader, std::string_view shaderType)
{
    // Check if the shader compiled successfully
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    // Display an error if the shader failed to compile
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::" << shaderType << "::COMPILATION_FAILED\n"
                  << infoLog << '\n';
    }
}

void checkShaderProgramLinkage(unsigned int &shaderProgram)
{
    // Check if the shader program linked successfully
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    // Display an error if the shader program failed to link
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER_PROGRAM::LINKAGE_FAILED\n"
                  << infoLog << '\n';
    }
}

Shader::Shader(const char *vertexPath, const char *fragmentPath)
{
    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // Ensure ifstream objects can throw exceptions
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // Open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        std::stringstream vShaderStream, fShaderStream;
        // Read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // Close file handlers
        vShaderFile.close();
        fShaderFile.close();

        // Convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n"
                  << e.code() << " " << e.what() << '\n';
    }
    const char *vShaderCode{vertexCode.c_str()};
    const char *fShaderCode{fragmentCode.c_str()};

    // 2. Compile shaders
    // Vertex shader
    unsigned int vertex{glCreateShader(GL_VERTEX_SHADER)};
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkShaderCompilation(vertex, "VERTEX");

    // Fragment shader
    unsigned int fragment{glCreateShader(GL_FRAGMENT_SHADER)};
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkShaderCompilation(fragment, "FRAGMENT");

    // Shader program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkShaderProgramLinkage(ID);

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::deleteShaderProgram()
{
    glDeleteProgram(ID);
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value)
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value)
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setFloat(const std::string &name, float value)
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat4(const std::string &name, float value0, float value1, float value2, float value3)
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), value0, value1, value2, value3);
}
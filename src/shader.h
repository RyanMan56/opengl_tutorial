#ifndef SHADER_H
#define SHADER_H

#include <glm/fwd.hpp>
#include <string>

class Shader
{
  public:
    // The program ID
    unsigned int ID;

    Shader(const char *vertexPath, const char *fragmentPath);

    // Use/activate the shader
    void use();

    void deleteShaderProgram();

    // Utility uniform functions
    void setBool(const std::string &name, bool value);
    void setInt(const std::string &name, int value);
    void setFloat(const std::string &name, float value);
    void setFloat4(const std::string &name, float value0, float value1,
                   float value2, float value3);
    void setMatrix4fv(const std::string &name, glm::mat4 &matrix);
};

#endif
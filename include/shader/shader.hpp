#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

class Shader {
public:
  // the program ID
  unsigned int ID;

  // Constructor to read and build shader from the shader paths of vertex and
  // fragment.
  Shader(const std::string &vertexPath, const std::string &fragmentPath);

  // use/activate the shader
  void use();

  // utility to set the uniforms
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
  void setVec4(const std::string &name, std::vector<float> value) const;
  void setVec3(const std::string &name, std::vector<float> value) const;
  void setMatrix(const std::string &name, glm::mat4) const;

private:
  unsigned int setupShader(GLenum shaderType, const std::string &shaderSource);
  unsigned int setupShaderProgram(unsigned int vertexShader,
                                  unsigned int fragmentShader);
  bool getShaderSource(std::string *vertexShaderSource,
                       const std::string &filename);
};

#endif

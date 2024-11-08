#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <shader/shader.hpp>
#include <sstream>
#include <string>
#include <unordered_map>
// Globals
#ifndef GLOBALS
#define GLOBALS
std::unordered_map<GLenum, const char *> shaderName = {
    {GL_VERTEX_SHADER, "VERTEX_SHADER"},
    {GL_FRAGMENT_SHADER, "FRAGMENT_SHADER"},
};

#endif // !GLOBALS

Shader ::Shader(const std::string &vertexPath,
                const std::string &fragmentPath) {
  std::string vertexShaderSource, fragmentShaderSrc;

  // Vertex Shader
  bool notHadError =
      getShaderSource(&vertexShaderSource, "shaders/" + vertexPath);
  if (!notHadError) {
    return;
  }
  unsigned int vertexShader = setupShader(GL_VERTEX_SHADER, vertexShaderSource);
  // Fragment Shader
  notHadError =
      getShaderSource(&fragmentShaderSrc, "shaders/" + fragmentPath);
  if (!notHadError) {
    return;
  }
  unsigned int fragmentShader =
      setupShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
  if (!vertexShader || !fragmentShader) {
    return;
  }

  unsigned int shaderProgram = setupShaderProgram(vertexShader, fragmentShader);
  if (!shaderProgram) {
    return;
  }
  ID = shaderProgram;

  // Shaders have been attahced to the program. They're are free to be deleted.
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

void Shader::use() { glUseProgram(ID); }

void Shader::setBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string &name, std::vector<float> value) const {
  if (value.size() < 3) {
    std::cout << "Cannot assign to a unifrom vec3f using a vector only "
                 "containing less than 3 values"
              << std::endl;
  }

  glUniform3f(glGetUniformLocation(ID, name.c_str()), value[0], value[1],
              value[2]);
}
void Shader::setVec4(const std::string &name, std::vector<float> value) const {
  if (value.size() <= 3) {
    std::cout << "Cannot assign to a unifrom vec4f using a vector only "
                 "containing 3 values"
              << std::endl;
  }

  glUniform4f(glGetUniformLocation(ID, name.c_str()), value[0], value[1],
              value[2], value[3]);
}

void Shader::setMatrix(const std::string &name, glm::mat4 matrix) const {

  glad_glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                          glm::value_ptr(matrix));
}

bool Shader ::getShaderSource(std::string *shaderSourceRef,
                              const std::string &filename) {
  std::ifstream shaderCodeFile;
  // These are differnet error bits in the format of one-hot encoding. So we can
  // safely use the OR operation here.
  shaderCodeFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    shaderCodeFile.open(filename);
    std::stringstream shaderCodeFileStream;
    shaderCodeFileStream << shaderCodeFile.rdbuf();
    shaderCodeFile.close();
    *shaderSourceRef = shaderCodeFileStream.str();
    return true;
  } catch (std::ifstream::failure e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSULLY_READ" << std::endl;
    std::cout << e.what() << "\nError Code: " << e.code() << std::endl;
    return false;
  }
}

unsigned int Shader::setupShaderProgram(unsigned int vertexShader,
                                        unsigned int fragmentShader) {
  unsigned int shaderProgram = glCreateProgram();
  int success;
  char *infoLog;
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR:SHADER::PROGRAM_LINK::COMPILATION_FAILED: \n%s"
              << infoLog;
    return 0;
  }
  return shaderProgram;
}

unsigned int Shader::setupShader(GLenum shaderType,
                                 const std::string &shaderSource) {
  unsigned int shader;
  shader = glCreateShader(shaderType);
  const char *shaderSourceChar = shaderSource.c_str();
  glShaderSource(shader, 1, &shaderSourceChar, NULL);
  glCompileShader(shader);
  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    printf("ERROR:SHADER:%s:COMPILATION_FAILED:\n%s\n", shaderName[shaderType],
           infoLog);
    return 0;
  }
  return shader;
}

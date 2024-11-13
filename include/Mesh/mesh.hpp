#include "shader/shader.hpp"
#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Vertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoord;
};

struct Texture {
  unsigned int id;
  std::string type;
  std::string filePath;
};

class Mesh {
public:
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  std::vector<Texture> textures;
  Mesh(Mesh &&other);
  Mesh(std::vector<Vertex> &&vertices, std::vector<uint32_t> &&indices,
       std::vector<Texture> &&textures);
  void Draw(Shader &shader);

private:
  uint32_t VAO, VBO, EBO;
  void SetupMesh();
};

#include <cstddef>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <Mesh/mesh.hpp>
#include <glad/glad.h>
#include <string>
using std::string;

Mesh::Mesh(std::vector<Vertex> &&vertices, std::vector<uint32_t> &&indices,
           std::vector<Texture> &&textures) {
  this->vertices = vertices;
  this->indices = indices;
  this->textures = textures;

  SetupMesh();
}
void Mesh::SetupMesh() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &VBO);
  // bind VAO
  glBindVertexArray(VAO);

  // bind VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0],
               GL_STATIC_DRAW);

  // bind EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t),
               &indices[0], GL_STATIC_DRAW);

  // Enable attributes
  // 1) Enable vertex positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Position));
  // 2) Enable Normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Normal));
  // 2) Enable Tex Coord
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Normal));
  glBindVertexArray(0);
}

void Mesh::Draw(Shader &shader) {
  uint32_t diffuseNr = 1;
  uint32_t specularNr = 1;

  for (int i = 0; i < textures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);

    // we have bound the ith texture in the array to the GPU shader's it Unit
    // now we need to bind that texture unit to the required material sampler
    string num = 0;
    if (textures[i].type == "texture_diffuse") {
      num = std::to_string(diffuseNr++);
    } else {
      num = std::to_string(specularNr++);
    }
    string texName = "material." + textures[i].type + num;
    shader.setInt(texName, i);
    glBindTexture(GL_TEXTURE_2D, textures[i].id);
  }
  glActiveTexture(GL_TEXTURE0);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
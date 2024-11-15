#include <cstddef>
#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <Mesh/mesh.hpp>
#include <glad/glad.h>
#include <string>
using std::cout;
using std::endl;
using std::string;
Mesh::Mesh(std::vector<Vertex> &&vertices, std::vector<uint32_t> &&indices,
           std::vector<Texture> &&textures, BaseColors baseColors) {
  this->vertices = vertices;
  this->indices = indices;
  this->textures = textures;
  this->baseColors.diffuseBase = baseColors.diffuseBase;
  this->baseColors.specularBase = baseColors.specularBase;
  SetupMesh();
}

Mesh::Mesh(Mesh &&other) {
  this->vertices = std::move(other.vertices);
  this->indices = std::move(other.indices);
  this->textures = std::move(other.textures);
  this->baseColors = other.baseColors;
  this->VAO = other.VAO;
  this->VBO = other.VBO;
  this->EBO = other.EBO;
}

void Mesh::SetupMesh() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  // 2) Enable Normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Normal));
  // 2) Enable Tex Coord
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, TexCoord));
  glBindVertexArray(0);
}

void Mesh::Draw(Shader &shader) {
  uint32_t diffuseNr = 0;
  uint32_t specularNr = 0;
  shader.use();
  for (int i = 0; i < textures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    // we have bound the ith texture in the array to the GPU shader's it Unit
    // now we need to bind that texture unit to the required material sampler
    string index;
    if (textures[i].type == "texture_diffuse") {
      index = "diffuseTexture[" + std::to_string(diffuseNr++) + "].texture";
    } else {
      index = "specularTexture[" + std::to_string(specularNr++) + "].texture";
    }
    string texName = "material." + index;
    // cout << "Assigning to " << texName << " the id of " << textures[i].id
    //      << endl;
    shader.setInt(texName, i);
    glBindTexture(GL_TEXTURE_2D, textures[i].id);
  }
  glActiveTexture(GL_TEXTURE0);
  shader.setInt("material.diffuseCount", diffuseNr);
  shader.setInt("material.specularCount", specularNr);
  // glm::vec3 bTest = {0.8, 0.8, 0.8};
  shader.setVec3("material.baseDiffuse", baseColors.diffuseBase);
  shader.setVec3("material.baseSpecular", baseColors.specularBase);
  // cout << "Using diffuse base of " << baseColors.diffuseBase.x << " "
  //      << baseColors.diffuseBase.y << " " << endl;
  //  cout << "Assigning base colors of " << baseColors.diffuseBase[0] << ", "
  //       << baseColors.diffuseBase[1] << ", " << baseColors.diffuseBase[2]
  //       << endl;
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

#include "assimp/material.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include <Model/model.hpp>
#include <iostream>
#include <stb_image/stb_image.h>
#include <string>
#include <vector>
using std::cout;
using std::endl;
using std::move;
using std::string;
using std::vector;
static int x = 0;
unsigned int loadAndSetupImage(const char *imageName);

void Model::Draw(Shader &shader) {
  for (int i = 0; i < meshes.size(); i++) {
    meshes[i].Draw(shader);
  }
}
void Model::loadModel(string path) {
  Assimp::Importer import;
  const aiScene *scene =
      import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    cout << "There was an error with scene creation: "
         << import.GetErrorString() << endl;
    return;
  }
  cout << "Model contains " << scene->mNumMeshes << " meshes" << endl;
  processNode(scene->mRootNode, scene);
  cout << "Scene processed " << x << " meshes." << endl;
}

void Model::processNode(aiNode *node, const aiScene *scene) {
  // process the meshes of the current node
  for (int i = 0; i < node->mNumMeshes; i++) {
    meshes.push_back(processMesh(scene->mMeshes[node->mMeshes[i]], scene));
    x += 1;
  }
  // process the children
  for (int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
  vector<Vertex> vertices;
  vector<uint32_t> indices;
  vector<Texture> textures;

  // process vertices
  for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
    Vertex vec;
    glm::vec3 pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y,
                              mesh->mVertices[i].z);
    vec.Position = pos;

    glm::vec3 normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y,
                                 mesh->mNormals[i].z);
    vec.Normal = normal;

    // mTextureCoords[0] applies for the entire mesh. It checks if it has any
    // texture coordinates for the entire mesh
    if (mesh->mTextureCoords[0]) {
      glm::vec2 texCoords =
          glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
      vec.TexCoord = texCoords;
    } else {
      glm::vec2 texCoords = glm::vec2(.0f, .0f);
      vec.TexCoord = texCoords;
    }
    vertices.push_back(vec);
  }


  // process indices

  for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (uint32_t j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  //// process materials
  // if (mesh->mMaterialIndex >= 0) {
  //   aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

  //  // 1) load the diffuseMap textures
  //  vector<Texture> diffuseMaps = loadMaterialTextures(
  //      material, aiTextureType_DIFFUSE, "texture_diffuse");

  //  // 2) load the specularMap textures
  //  vector<Texture> specularMaps = loadMaterialTextures(
  //      material, aiTextureType_SPECULAR, "texture_specular");
  //  textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
  //  textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
  //}
  return Mesh(move(vertices), move(indices), move(textures));
}

vector<Texture> &&Model::loadMaterialTextures(aiMaterial *mat,
                                              aiTextureType type,
                                              string typeName) {
  vector<Texture> textures;
  for (uint32_t i = 0; i < mat->GetTextureCount(type); i++) {
    aiString texPath;
    mat->GetTexture(type, i, &texPath);
    Texture texture;
    texture.type = typeName;
    // texture.path = texPath;
    texture.id = loadAndSetupImage(texPath.C_Str());
  }
  return move(textures);
}

// This function loads the image using the stb library, and binds it to a
// newly created texture object. Then, it uses GL to generate mipmap after
// setting up the GL-parameters
unsigned int loadAndSetupImage(const char *imageName) {
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  bool containsAlpha = false;

  int width, height, nrChannel;
  // nr channel: color channel length
  // 0: R Channel I think?
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load(imageName, &width, &height, &nrChannel, 0);
  if (!data) {
    std::cout << "Error loading up the image!";
    return 0;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
               containsAlpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  // set the texture wrapping/filtering options (on the currently bound
  // texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(data);
  return texture;
}

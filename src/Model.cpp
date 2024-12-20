#include "assimp/material.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "shader/shader.hpp"
#include <MacTypes.h>
#include <Model/model.hpp>
#include <camera/camera.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <imgui/imgui.h>
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
extern Shader *outlineShader;

void Model::Draw(Shader &shader, Camera *camera) {
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  if (!hasOutline) {
    for (Mesh &m : meshes) {
      m.Draw(shader);
    }
    return;
  }

  /**
  Draw Outline:
    - First fill up the stencil buffer with the object in view space. The
  stencil shouldn't filter anything this time.
    - Use this filled up stencil buffer to create an outline
  */
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 1, 0xff); // always pass any fragments
  glStencilMask(0xFF);
  for (Mesh &m : meshes) {
    m.Draw(shader);
  }

  glStencilFunc(GL_NOTEQUAL, 1, 0xff); // only pass fragments not in 1
  glStencilMask(0x00);                 // No writing this time
  glDisable(GL_DEPTH_TEST);
  glm::vec3 oldScale = this->scale;
  // scale += 0.02;
  UpdateShaderTransforms(camera, outlineShader);
  if (!outlineShader) {
    cout << "NO OUTLINE SHADER!";
    exit(1);
  }
  outlineShader->setFloat("outlineAmt", 1.0f);
  for (Mesh &m : meshes) {
    m.Draw(*outlineShader);
  }
  // scale -= 0.02;

  // reset stencil setup
  glStencilMask(0xff);
  glEnable(GL_DEPTH_TEST);
  glStencilFunc(GL_ALWAYS, 1, 0xff);
}
void Model::loadModel(string path, bool shouldFlipUVs) {
  Assimp::Importer import;
  const aiScene *scene = import.ReadFile(
      path, aiProcess_Triangulate | ((shouldFlipUVs) ? aiProcess_FlipUVs : 0));

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    cout << "There was an error with scene creation: "
         << import.GetErrorString() << endl;
    return;
  }
  directory = path.substr(0, path.find_last_of('/'));
  processNode(scene->mRootNode, scene);
  modelName = path;
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
  aiColor3D colorDiffuse(0), colorSpecular(0);
  if (mesh->mMaterialIndex >= 0) {
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    // 1) load the diffuseMap textures
    vector<Texture> diffuseMaps = loadMaterialTextures(
        material, aiTextureType_DIFFUSE, "texture_diffuse");
    material->Get(AI_MATKEY_COLOR_DIFFUSE, colorDiffuse);

    // 2) load the specularMap textures
    vector<Texture> specularMaps = loadMaterialTextures(
        material, aiTextureType_SPECULAR, "texture_specular");
    material->Get(AI_MATKEY_COLOR_SPECULAR, colorSpecular);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
  }
  BaseColors base{};
  base.diffuseBase = glm::vec3(colorDiffuse.r, colorDiffuse.g, colorDiffuse.b);
  base.specularBase =
      glm::vec3(colorSpecular.r, colorSpecular.g, colorSpecular.b);
  return Mesh(move(vertices), move(indices), move(textures), base);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                            string typeName) {
  vector<Texture> textures;
  for (uint32_t i = 0; i < mat->GetTextureCount(type); i++) {
    aiString texPath;
    mat->GetTexture(type, i, &texPath);
    aiTextureOp op;
    mat->Get(AI_MATKEY_TEXOP(type, i), op);

    string fullPath = (directory + '/' + string(texPath.C_Str()));
    bool skip = false;
    for (Texture t : loadedTextures) {
      if (t.filePath == fullPath) {
        textures.push_back(t);
        skip = true;
        break;
      }
    }
    if (skip)
      continue;
    Texture texture;
    texture.type = typeName;
    // texture.path = texPath;
    texture.filePath = fullPath;
    texture.id = loadAndSetupImage(fullPath.c_str());
    cout << "ID is => " << texture.id << endl;
    textures.push_back(texture);
    loadedTextures.push_back(texture);
  }
  return textures;
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
  cout << "Loading " << imageName << endl;
  unsigned char *data = stbi_load(imageName, &width, &height, &nrChannel, 0);
  if (!data) {
    std::cout << "Error loading up the image! Image has " << nrChannel
              << " Channels " << std::endl;
    return 0;
  }
  if (nrChannel > 3) {
    cout << "Alpha channel in texture! " << endl;
    containsAlpha = true;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, containsAlpha ? GL_RGBA : GL_RGB, width,
               height, 0, containsAlpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE,
               data);
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

string printVec3(glm::vec3 vec) {
  return "{" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " +
         std::to_string(vec.z) + "} ";
}
void Model::DisplayWindow() {
  if (modelName == "empty")
    return;
  bool displayWindow = true;
  ImGui::Begin(("Model: " + modelName).c_str(), &displayWindow);
  ImGui::Text("Transform");
  ImGui::Text("%s", ("Position" + printVec3(position)).c_str());
  ImGui::Text("Rotation");
  ImGui::Text("%s", ("Scale" + printVec3(scale)).c_str());
  ImGui::Text("Materials");
  ImGui::End();
}

void Model::UpdateShaderTransforms(Camera *camera) {
  this->UpdateShaderTransforms(camera, this->shader);
}

void Model::UpdateShaderTransforms(Camera *camera, Shader *shader) {
  if (!shader) {
    std::cerr << "No Shader to update!" << std::endl;
    return;
  }
  shader->use();
  glm::mat4 modelMat(1.0f);
  modelMat = glm::translate(modelMat, this->position);
  modelMat = glm::scale(modelMat, scale);
  shader->setMatrix("model", modelMat);
  shader->setMatrix("view", camera->view);
  shader->setFloat("nearPlane", camera->nearPlane);
  shader->setFloat("farPlane", camera->farPlane);
  shader->setVec3("viewPos", camera->cameraPos);
  shader->setMatrix("projection", camera->projection);
}

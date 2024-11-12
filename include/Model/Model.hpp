#include <Mesh/mesh.hpp>
#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <set>
#include <string>
#include <vector>
using std::set;
using std::string;
using std::vector;

class Model {
public:
  Model(string filePath) { loadModel(filePath); }
  void Draw(Shader &shader);

private:
  vector<Mesh> meshes; // to be filled when processing tree
  string directory;

  void loadModel(string filePath);
  void processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);
  vector<Texture> loadedTextures;

  vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                       string typeName);
};
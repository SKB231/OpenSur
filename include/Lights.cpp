#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <shader/shader.hpp>
#include <string>
using std::string;
using std::vector;

enum LightType {
  DIRECTIONAL_LIGHT = 1,
  POINT_LIGHT = 2,
  SPOT_LIGHT = 3,
  GENERAL = 0,
};

vector<float> ConvertToVector(float *glmVec) {
  return {glmVec[0], glmVec[1], glmVec[2]};
}

struct DirectionalLight {
  float direction[3];
  float ambient[3], diffuse[3], specular[3];
  bool IsStatic;
  void UpdateShaderValues(Shader *shader) {
    shader->use();
    shader->setVec3("dirLight.ambient", ambient);
    shader->setVec3("dirLight.diffuse", diffuse);
    shader->setVec3("dirLight.specular", specular);
    shader->setVec3("dirLight.direction", direction);
  }
  void DisplayWindow() {
    bool showWindow = true;
    ImGui::Begin("Background Color", &showWindow);
    ImGui::ColorEdit4("Directional light DIRECTION ", direction);
    ImGui::ColorEdit4("Directional light DIFFUSE ", diffuse);
    ImGui::ColorEdit4("Directional light SPECULAR ", specular);
    ImGui::ColorEdit4("Directional light AMBIENT", ambient);
    ImGui::End();
  }
};

struct PointLight {
  float ambient[3], diffuse[3], specular[3];
  float position[3];
  float constant, linear, quadratic;
  int lightCount = 0;
  bool IsStatic;
  void UpdateShaderValues(Shader *shader) {
    shader->use();
    shader->setVec3("pointLights[" + std::to_string(lightCount) + "].ambient",
                    ambient);
    shader->setVec3("pointLights[" + std::to_string(lightCount) + "].diffuse",
                    diffuse);
    shader->setVec3("pointLights[" + std::to_string(lightCount) + "].specular",
                    specular);
    shader->setVec3("pointLights[" + std::to_string(lightCount) + "].position",
                    position);
    shader->setFloat("pointLights[" + std::to_string(lightCount) + "].constant",
                     constant);
    shader->setFloat("pointLights[" + std::to_string(lightCount) + "].linear",
                     linear);
    shader->setFloat(
        "pointLights[" + std::to_string(lightCount) + "].quadratic", quadratic);
  }
};

// struct SpotLight : public Light {
//   const static LightType type{SPOT_LIGHT};
//   glm::vec3 position, direction;
//   float constant, linear, quadratic, cutoff, outerConeCutoff;
// };
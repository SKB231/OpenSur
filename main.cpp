#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <camera/camera.hpp>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <memory>
#include <shader/shader.hpp>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include <Model/model.hpp>
#include <format>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <stb_image/stb_image.h>

using std::cout;
using std::endl;
using std::string;

// Globals
#ifndef GLOBALS
#define GLOBALS
float f32_zero = -10.0f;
float f32_one = 10.0f;
bool show_demo_window = true;
int WIDTH = 1000;
int HEIGHT = 1000;
const char *glsl_version = "#version 150";
float fov = 45.0;
float lastTime = 0.0f;
float deltaTime = 1.0f;
bool firstMouse = true;
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

bool enableControl = true;
bool displayCursor = false;
bool displayingCursor = false;

extern std::unique_ptr<Camera> activeCamera;

enum ShaderType {
  vertexShader = GL_VERTEX_SHADER,
  fragShader = GL_FRAGMENT_SHADER,
};

glm::vec3 direction;
#endif // !GLOBALS
// Function defintions
glm::vec3 cubePositions[] = {
    glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
    glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};
std::vector<float> pointLightPositions[] = {
    std::vector<float>{0.7f, 0.2f, 2.0f},
    std::vector<float>{2.3f, -3.3f, -4.0f},
    std::vector<float>{-4.0f, 2.0f, -12.0f},
    std::vector<float>{0.0f, 0.0f, -3.0f}};

void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void makeCube(unsigned int *vao);
GLFWwindow *setupWindow();
unsigned int setupShader(GLenum shaderType, const char *shaderSource);
unsigned int setupShaderProgram(unsigned int vertexShader,
                                unsigned int fragmentShader);
bool getShaderSource(std::string *vertexShaderSource, const char *filename);
unsigned int loadAndSetupImage(const char *imageName, bool containsAlpha);
void makeCube(unsigned int *vao);

int main() {

  // setup window
  GLFWwindow *window = setupWindow();

  if (!window) {
    return 1;
  }

  activeCamera = std::make_unique<Camera>(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // setup shaders
  Shader simpleShader("vertexShader.glsl", "whiteFrag.glsl");
  Shader lightingShader("vertexShader.glsl", "simpleLight.glsl");

  Camera::InitCallbacks(window);

  // Imgui Context init:
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad;            // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
  io.ConfigFlags |=
      !ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport /

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
  ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
  ImGui_ImplOpenGL3_Init(glsl_version);
  // RenderLoop:
  bool pointLightsOn = true;
  bool prevPointLightsOn = false;
  Model backpackModel("backpack/backpack.obj");
  float background[3] = {0.5f, 0.5f, 0.5f};
  float dirLightDirection[3] = {-0.2f, -1.0f, -0.3f};
  float dirLightDiffuse[3] = {0.5f, 0.5f, 0.5f};
  float dirLightSpecular[3] = {1.0f, 1.0f, 1.0f};
  float dirLightAmbient[3] = {0.2f, 0.2f, 0.2f};

  lightingShader.use();
  lightingShader.setInt("usePointLight", 0);
  lightingShader.setVec3(
      "dirLight.direction",
      std::vector<float>(dirLightDirection, dirLightDirection + 3));
  lightingShader.setVec3(
      "dirLight.ambient",
      std::vector<float>(dirLightAmbient, dirLightAmbient + 3));
  lightingShader.setVec3(
      "dirLight.diffuse",
      std::vector<float>(dirLightDiffuse, dirLightDiffuse + 3));
  lightingShader.setVec3(
      "dirLight.specular",
      std::vector<float>(dirLightSpecular, dirLightSpecular + 3));
  lightingShader.setFloat("material.shininess", 32.0f);
  glEnable(GL_DEPTH_TEST);

  uint32_t tinyCubeVAO;
  makeCube(&tinyCubeVAO);

  // add point lights
  lightingShader.use();
  for (int i = 0; i < 4; i++) {
    // http://www.ogre3d.org/tikiwiki/tiki-index.php?page=-Point+Light+Attenuation
    lightingShader.setVec3("pointLights" + index + ".position",
                           pointLightPositions[i]);
    lightingShader.setVec3("pointLights" + index + ".ambient",
                           {0.2f, 0.2f, 0.2f});
    lightingShader.setVec3("pointLights" + index + ".diffuse",
                           {0.5f, 0.5f, 0.5f});

    lightingShader.setVec3("pointLights" + index + ".specular", {1, 1, 1});
    lightingShader.setFloat("pointLights" + index + ".constant", 1.0f);
    lightingShader.setFloat("pointLights" + index + ".linear", 0.09f);
    lightingShader.setFloat("pointLights" + index + ".quadratic", 0.032f);
  }
  lightingShader.setInt("usePointLight", 1);

  while (!glfwWindowShouldClose(window)) {

    // Handle any polled user inputs:
    processInput(window);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    bool showDirLightWind = true;
    if (pointLightsOn != prevPointLightsOn) {
      prevPointLightsOn = pointLightsOn;
      // lightingShader.setInt("usePointLight", pointLightsOn);
    }
    // lightingShader.setVec3(
    //  "dirLight.direction",
    //  std::vector<float>(dirLightDirection, dirLightDirection + 3));
    if (show_demo_window)
      ImGui::ShowDemoWindow(&show_demo_window);
    bool showWidnow = true;
    activeCamera->DisplayCameraProperties();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(background[0], background[1], background[2], 1);

    ImGui::Begin("Background Color", &showWidnow);
    ImGui::ColorEdit4("Background: ", background);
    ImGui::ColorEdit4("Directional light DIRECTION ", dirLightDirection);
    ImGui::ColorEdit4("Directional light DIFFUSE ", dirLightDiffuse);
    ImGui::ColorEdit4("Directional light SPECULAR ", dirLightSpecular);
    ImGui::ColorEdit4("Directional light AMBIENT", dirLightAmbient);
    ImGui::End();

    // Update time and delta time
    float timeValue = glfwGetTime();
    deltaTime = timeValue - lastTime;
    lastTime = timeValue;

    // Update camera matrices
    activeCamera->UpdateCamera();

    std::vector<float> cameraPos = {activeCamera->cameraPos.x,
                                    activeCamera->cameraPos.y,
                                    activeCamera->cameraPos.z};
    lightingShader.use();
    lightingShader.setVec3("viewPos", cameraPos);
    lightingShader.setMatrix("view", activeCamera->view);
    lightingShader.setMatrix("projection", activeCamera->projection);
    lightingShader.setMatrix("model", glm::mat4(1.0f));
    lightingShader.setVec3(
        "dirLight.direction",
        std::vector<float>(dirLightDirection, dirLightDirection + 3));
    lightingShader.setVec3(
        "dirLight.ambient",
        std::vector<float>(dirLightAmbient, dirLightAmbient + 3));
    lightingShader.setVec3(
        "dirLight.diffuse",
        std::vector<float>(dirLightDiffuse, dirLightDiffuse + 3));
    lightingShader.setVec3(
        "dirLight.specular",
        std::vector<float>(dirLightSpecular, dirLightSpecular + 3));
    backpackModel.Draw(lightingShader);
    // Draw the white cube : Update camera information, and model matrix
    simpleShader.use();
    glBindVertexArray(tinyCubeVAO);
    simpleShader.setMatrix("view", activeCamera->view);
    simpleShader.setMatrix("projection", activeCamera->projection);
    for (int i = 0; i < 4; i++) {
      glm::mat4 modelLight = glm::mat4(1.0f);
      modelLight =
          glm::translate(modelLight, glm::vec3(pointLightPositions[i][0],
                                               pointLightPositions[i][1],
                                               pointLightPositions[i][2]));
      modelLight = glm::scale(modelLight, glm::vec3(0.2f));
      simpleShader.setMatrix("model", modelLight);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // Render Imgui window
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we
    // The way I understand it is if the window is weird, this will handle it
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      GLFWwindow *backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }

    // Swap the color buffer with the current buffer being displayed.
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();
  return 0;
}

// This function loads the image using the stb library, and binds it to a newly
// created texture object. Then, it uses GL to generate mipmap after setting up
// the GL-parameters
unsigned int loadAndSetupImage(const char *imageName, bool containsAlpha) {
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

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

  // set the texture wrapping/filtering options (on the currently bound texture
  // object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(data);
  return texture;
}

GLFWwindow *setupWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);

  if (window == NULL) {
    std::cout << "Failed to create GLFW window\n";
    glfwTerminate();
    return NULL;
  }
  glfwMakeContextCurrent(window);

  // Initialize GLAD to allow it to manage Os specific functions that implement
  // the OpenGL specifications
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD!\n";
    glfwTerminate();
    return NULL;
  }

  /**
   * Set the size of the renderring window so OpenGL knows what area within our
   * sized window it should render in. For our case, we render in all of it.
   */
  int fwidth, fheight;
  glfwGetFramebufferSize(window, &fwidth, &fheight);
  glViewport(0, 0, fwidth, fheight);

  glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
  return window;
}

void processInput(GLFWwindow *window) {
  const float cameraSpeed = 2.5f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, 1);

  if (!enableControl && glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
    enableControl = true;
  if (enableControl && glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
    enableControl = false;
  if (!displayCursor && glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
    displayCursor = true;

  if (displayCursor && glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
    displayCursor = false;

  if (displayCursor != displayingCursor) {
    glfwSetInputMode(window, GLFW_CURSOR,
                     (displayCursor) ? GLFW_CURSOR_NORMAL
                                     : GLFW_CURSOR_DISABLED);
    displayingCursor = displayCursor;
  }
  if (!enableControl)
    return;
  // make our camera process the rest of the inputs
  activeCamera->KeyInput(window);
}

void frameBufferSizeCallback(GLFWwindow *window, int width, int height) {
  int fwidth, fheight;
  glfwGetFramebufferSize(window, &fwidth, &fheight);
  glViewport(0, 0, fwidth, fheight);
  WIDTH = fwidth;
  HEIGHT = fheight;
}

void makeCube(unsigned int *vao) {
  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float vertices[] = {
      // positions          // normals           // texture coords
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,
      0.0f,  -1.0f, 1.0f,  1.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f,  -0.5f,
      0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      0.0f,  1.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,
      -0.5f, -1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
      0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
      -0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
      0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
      -1.0f, 0.0f,  1.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
      1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
      -0.5f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      1.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f};

  unsigned VBO;
  glGenVertexArrays(1, vao);
  glBindVertexArray(*vao);

  // unsigned int EBO;
  // glGenBuffers(1, &EBO);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
  //              GL_STATIC_DRAW);

  // Generate a buffer and assign the ID to VBO: Vertex Buffer Object.
  glGenBuffers(1, &VBO);

  // Bind the GL_ARRAY_BUFFER with the VBO Buffer.
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // Copies vertex data to buffer we have just bound
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Setup the vertex objects for our VBOs
  // first_arg: Position we are assigning. We set it to 0 since we are assigning
  // the position attribute which is the 0th attribute. second_arg: How many
  // components per vertex attribute.
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // Similarly, set the attributes of the second index, which are the colors,
  // and enable them.
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  // Lastly, set the attributes of the third index, which are the texture
  // coordinates.
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
}
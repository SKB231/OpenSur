#include <camera/camera.hpp>
#include<iostream>
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
#include <stb_image/stb_image.h>

// Globals
#ifndef GLOBALS
#define GLOBALS
int WIDTH = 800;
int HEIGHT = 600;
float fov = 45.0;
float lastTime = 0.0f;
float deltaTime = 1.0f;
bool firstMouse = true;
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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

void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void makeCube(unsigned int *vao);
GLFWwindow *setupWindow();
unsigned int setupShader(GLenum shaderType, const char *shaderSource);
unsigned int setupShaderProgram(unsigned int vertexShader,
                                unsigned int fragmentShader);
bool getShaderSource(std::string *vertexShaderSource, const char *filename);
unsigned int loadAndSetupImage(const char *imageName, bool containsAlpha);

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

  unsigned int VAO1;
  makeCube(&VAO1);

  unsigned int VAO2;
  makeCube(&VAO2);

  glEnable(GL_DEPTH_TEST);
  lightingShader.use();
  lightingShader.setVec3("objectColor", {1.0f, 0.5f, 0.31f});
  lightingShader.setVec3("lightColor",  {1.0f, 1.0f, 1.0f});
  lightingShader.setVec3("lightPos", {lightPos[0], lightPos[1], lightPos[2]});
  Camera::InitCallbacks(window);

  // RenderLoop:
  while (!glfwWindowShouldClose(window)) {

    // Handle any polled user inputs:
    processInput(window);

    // Setup scene background
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update time and delta time
    float timeValue = glfwGetTime();
    deltaTime = timeValue - lastTime;
    lastTime = timeValue;


    // Update camera matrices
    activeCamera->UpdateCamera();

    // Draw the cubes
    std::vector<float> cameraPos = {activeCamera->cameraPos.x, activeCamera->cameraPos.y, activeCamera->cameraPos.z};
    using namespace std;
    cout << "Sending " << cameraPos[0] << " " << cameraPos[1] << " " << cameraPos[2] << endl;
    lightingShader.setVec3("viewPos", cameraPos);

    // Draw the white cube: Update camera information, and model Matrix
    simpleShader.use();
    glBindVertexArray(VAO1);
    simpleShader.setMatrix("view", activeCamera->view);
    simpleShader.setMatrix("projection", activeCamera -> projection);
    glm::mat4 modelLight = glm::mat4(1.0f);
    modelLight = glm::translate(modelLight, lightPos);
    modelLight = glm::scale(modelLight, glm::vec3(0.2f));
    simpleShader.setMatrix("model", modelLight);
    glad_glDrawArrays(GL_TRIANGLES, 0, 36);



    // Draw the regular cube: Update camera information, and model Matrix
    lightingShader.use();
    glBindVertexArray(VAO2); // setup a light VAO object
    lightingShader.setMatrix("view", activeCamera->view);
    lightingShader.setMatrix("projection", activeCamera -> projection);
    glm::mat4 model = glm::mat4(1.0f);
    lightingShader.setMatrix("model", model);
    glad_glDrawArrays(GL_TRIANGLES, 0, 36);

    // Swap the color buffer with the current buffer being displayed.
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
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
  glBindTexture(GL_TEXTURE_2D, 0);

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
  glViewport(0, 0, WIDTH, HEIGHT);

  glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
  return window;
}

void makeCube(unsigned int *vao) {
  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float vertices[] = {
      -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
       0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
       0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
       0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
      -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
      -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

      -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
       0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
       0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
       0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
      -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

      -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
      -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
      -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
      -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

       0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
       0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
       0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
       0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
       0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
       0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

      -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
       0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
       0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
       0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

      -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
       0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
       0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
       0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
  };
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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Similarly, set the attributes of the second index, which are the colors,
  // and enable them.
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

void processInput(GLFWwindow *window) {
  const float cameraSpeed = 2.5f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, 1);

  // make our camera process the rest of the inputs
  activeCamera->KeyInput(window);
}

void frameBufferSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}
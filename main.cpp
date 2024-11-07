#include "include/glad/glad.h"
#include "shader.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Globals
#ifndef GLOBALS
#define GLOBALS
int WIDTH = 800;
int HEIGHT = 600;

enum ShaderType {
  vertexShader = GL_VERTEX_SHADER,
  fragShader = GL_FRAGMENT_SHADER,
};

#endif // !GLOBALS
// Function defintions
glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f), 
    glm::vec3( 2.0f,  5.0f, -15.0f), 
    glm::vec3(-1.5f, -2.2f, -2.5f),  
    glm::vec3(-3.8f, -2.0f, -12.3f),  
    glm::vec3( 2.4f, -0.4f, -3.5f),  
    glm::vec3(-1.7f,  3.0f, -7.5f),  
    glm::vec3( 1.3f, -2.0f, -2.5f),  
    glm::vec3( 1.5f,  2.0f, -2.5f), 
    glm::vec3( 1.5f,  0.2f, -1.5f), 
    glm::vec3(-1.3f,  1.0f, -1.5f)  
};

void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void makeCube(unsigned int *vao, float offset);
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

  // load textures
  unsigned int texture1 = loadAndSetupImage("container.jpg", false);
  unsigned int texture2 = loadAndSetupImage("awesomeface.png", true);

  // setup shaders
  Shader simpleShader("vertexShader.glsl", "fragmentShader.glsl");

  unsigned int VAO1;
  makeCube(&VAO1, 0);

  // Model Matrix: Local Space -> World Space
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

  // View matrix: World Space -> View Space
  glm::mat4 view = glm::mat4(1.0f);
  view = glm::translate(view, glm::vec3(.0f, .0f, -3.0f));

  // Projection Matrix
  glm::mat4 projection =
      glm::perspective(glm::radians(90.0f), 800 / 600.0f, 0.1f, 100.0f);
  
  glEnable(GL_DEPTH_TEST);
  simpleShader.use();
  simpleShader.setInt("texture1", 0);
  simpleShader.setInt("texture2", 1);
  simpleShader.setMatrix("model", model);
  simpleShader.setMatrix("view", view);
  simpleShader.setMatrix("projection", projection);

  // RenderLoop:
  while (!glfwWindowShouldClose(window)) {

    // Handle any polled user inputs:
    processInput(window);

    // Rendering
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Activate shader program
    simpleShader.use();

    // draw shape.
    float timeValue = glfwGetTime();
    float blueValue = (sin(timeValue) / 2.0f) + 0.5f;
    // int vertexColorLocation = glGetUniformLocation(shaderProgram,
    // "ourColor");



    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glBindVertexArray(VAO1);
    //glDrawElements(GL_POINTS, 36, GL_UNSIGNED_INT, 0);
	//glDrawElements(GL_POINTS, 0, ;
	// model = glm::rotate(model, glm::radians(1.0f), glm::vec3(0.5, 1.0f, 0.0f));
	for(int i = 0;i < 10; i++) {
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(model, cubePositions[i]);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(20.0f) * (i%3 == 0 ? timeValue : 1.0f), glm::vec3(0, 1.0, 0.0));
    	simpleShader.setMatrix("model", modelMatrix);
		glad_glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	//glClear(GL_COLOR_BUFFER_BIT)
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

void makeCube(unsigned int *vao, float offset) {
  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float vertices[] = {
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

      -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};

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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Similarly, set the attributes of the second index, which are the colors,
  // and enable them.
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

void frameBufferSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }
}
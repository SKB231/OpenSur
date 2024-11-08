#ifndef CAMERA_H
#define CAMERA_H

#include <shader/shader.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>

class Camera;
inline float lastX;
inline float lastY;
inline std::unique_ptr<Camera> activeCamera;

class Camera {
public:
  float pitch = -130.0f, yaw = -40, fov = 45.0f;
  float cameraSpeed = 0.1f;
  glm::vec3 cameraPos = glm::vec3(3.0f, 3.0f, 3.0f);
  glm::vec3 cameraFront = glm::vec3(.0f, .0f, -1.0f);
  glm::vec3 cameraUp = glm::vec3(.0f, 1.0f, 0.0f);
  glm::vec3 direction;
  glm::mat4 view;
  glm::mat4 projection;
  bool firstMouse = true;
  Camera(GLFWwindow *window);
  static void InitCallbacks(GLFWwindow *window);
  static void MouseCallback(GLFWwindow *window, double xpos, double ypos);
  static void ScrollCallback(GLFWwindow *window, double xpos, double ypos);
  void UpdateCamera();
  void KeyInput(GLFWwindow *window);
};

#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>


// Function defintions
void frameBufferSizeCallback(GLFWwindow* window,int width,int height);
void processInput(GLFWwindow* window);


int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);

	if (window == NULL) {
		printf("Failed to create GLFW window\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLAD to allow it to manage Os specific functions that implement the OpenGL specifications
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize GLAD!\n");
		glfwTerminate();
		return -1;
	}


	/** 
	* Set the size of the renderring window so OpenGL knows what area within our sized window it should render in.
	* For our case, we render in all of it.	
	*/
	glViewport(0,0, 800, 600);

	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
	

	// RenderLoop:
	while(!glfwWindowShouldClose(window)) {

		// Handle any polled user inputs:
		processInput(window);


		// Rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Swap the color buffer with the current buffer being displayed.
		glfwSwapBuffers(window);

		// Poll all the user events that have occured.
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}


void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window) {
	if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, 1);
	}
}

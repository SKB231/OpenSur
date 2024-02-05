#include "shader.h"
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
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

void frameBufferSizeCallback(GLFWwindow* window,int width,int height);
void processInput(GLFWwindow* window);
void makeTriangle(unsigned int* vao, float offset);
GLFWwindow* setupWindow();
unsigned int setupShader(GLenum shaderType, const char* shaderSource);
unsigned int setupShaderProgram(unsigned int vertexShader, unsigned int fragmentShader);
bool getShaderSource(std::string* vertexShaderSource, const char* filename);
unsigned int loadAndSetupImage(const char* imageName, bool containsAlpha);

int main() {
	GLFWwindow* window = setupWindow();
	if(!window) {
		return 1;
	}
	
	int width, height, nrChannel; 

	// nr channel: color channel length
	// 0: R Channel I think?
	unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannel, 0);
	unsigned int texture;	
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);


	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if(data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load image data"; 
	}


	stbi_image_free(data);

	unsigned int texture2 = loadAndSetupImage("awesomeface.png", true);

	Shader simpleShader("vertexShader.glsl", "fragmentShader.glsl");
	unsigned int VAO1;
	makeTriangle(&VAO1, 0);
	
	simpleShader.use();
	simpleShader.setInt("texture1", 0);
	simpleShader.setInt("texture2", 1);

	// RenderLoop:
	while(!glfwWindowShouldClose(window)) {


		// Handle any polled user inputs:
		processInput(window);

		// Rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// Activate shader program
		simpleShader.use();


		// draw shape.
		float timeValue = glfwGetTime();
		float blueValue = (sin(timeValue)/2.0f) + 0.5f;
		// int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		glBindVertexArray(VAO1);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// Swap the color buffer with the current buffer being displayed.
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

// This function loads the image using the stb library, and binds it to a newly created texture object. Then, it uses GL to generate mipmap after setting up the GL-parameters
unsigned int loadAndSetupImage(const char* imageName, bool containsAlpha) {
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

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, containsAlpha ? GL_RGBA: GL_RGB , GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);
	return texture;

}

GLFWwindow* setupWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);

	if (window == NULL) {
		std::cout<<"Failed to create GLFW window\n";
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLAD to allow it to manage Os specific functions that implement the OpenGL specifications
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout<<"Failed to initialize GLAD!\n";
		glfwTerminate();
		return NULL;
	}
		
	/** 
	* Set the size of the renderring window so OpenGL knows what area within our sized window it should render in.
	* For our case, we render in all of it.	
	*/
	glViewport(0,0, WIDTH, HEIGHT);

	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
	return window;
}

void makeTriangle(unsigned int* vao, float offset) {
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
    	float vertices[] = {
    	    // positions          // colors           // texture coords
    	     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
    	     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
    	    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
    	    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    	};
    	unsigned int indices[] = {  
    	    0, 1, 3, // first triangle
    	    1, 2, 3  // second triangle
    	};

	unsigned VBO;
	glGenVertexArrays(1, vao);
	glBindVertexArray(*vao);


	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	// Generate a buffer and assign the ID to VBO: Vertex Buffer Object.
	glGenBuffers(1, &VBO);
	
	// Bind the GL_ARRAY_BUFFER with the VBO Buffer.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Copies vertex data to buffer we have just bound
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Setup the vertex objects for our VBOs
	// first_arg: Position we are assigning. We set it to 0 since we are assigning the aPos which is the 0th attribute.
	// second_arg: How many components per vertex attribute.
	// third_argument: What type of data are we sending? GL_FLOAT
	// 4th arg: Is our data normalized? No
	// 5th arg: 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);

	// Similarly, set the attributes of the second index, which are the colors, and enable them.
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float),(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);


	// Lastly, set the texture coordinate data:
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float),(void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);
}

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window) {
	if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, 1);
	}
}


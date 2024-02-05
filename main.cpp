#include <cmath>
#include <cstddef>
#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <unordered_map>

// Globals
int WIDTH = 800;
int HEIGHT = 600;

enum ShaderType {
	vertexShader = GL_VERTEX_SHADER,
	fragShader = GL_FRAGMENT_SHADER,
};

std::unordered_map<GLenum, const char*> shaderName = {
	{GL_VERTEX_SHADER, "VERTEX_SHADER"},
	{GL_FRAGMENT_SHADER, "FRAGMENT_SHADER"}, 
};

// Function defintions

void frameBufferSizeCallback(GLFWwindow* window,int width,int height);
void processInput(GLFWwindow* window);
void makeTriangle(unsigned int* vao, float offset);
GLFWwindow* setupWindow();
unsigned int setupShader(GLenum shaderType, const char* shaderSource);
unsigned int setupShaderProgram(unsigned int vertexShader, unsigned int fragmentShader);
bool getShaderSource(std::string* vertexShaderSource, const char* filename);

int main() {
	GLFWwindow* window = setupWindow();
	if(!window) {
		return 1;
	}
	
	std::string vertexShaderSource, fragmentShaderSrc;
	bool notHadError = getShaderSource(&vertexShaderSource,"vertexShader.glsl");
	if(!notHadError) {
		return 1;
	}
	unsigned int vertexShader = setupShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());
	notHadError = getShaderSource(&fragmentShaderSrc,"fragmentShader.glsl");
	if(!notHadError) {
		return 1;
	}
	unsigned int fragmentShader = setupShader(GL_FRAGMENT_SHADER, fragmentShaderSrc.c_str());
	unsigned int shaderProgram = setupShaderProgram(vertexShader,fragmentShader);
	glUseProgram(shaderProgram);
	unsigned int VAO1;
	makeTriangle(&VAO1, 0);

	// RenderLoop:
	while(!glfwWindowShouldClose(window)) {


		// Handle any polled user inputs:
		processInput(window);

		// Rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// Activate shader program
		glUseProgram(shaderProgram);


		// draw shape.
		float timeValue = glfwGetTime();
		float blueValue = (sin(timeValue)/2.0f) + 0.5f;
		int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		glUniform4f(vertexColorLocation, 0.0f, 0.0f, blueValue,1.0f);

		glBindVertexArray(VAO1);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		// Swap the color buffer with the current buffer being displayed.
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

bool getShaderSource(std::string* shaderSourceRef, const char* filename) {
	FILE* fs = fopen(filename, "r");

	char buf[512];

	if(!fs) {
		std::cout << "Error opening file: "<< filename;
		return false;
	}
	
	while(std::fgets(buf, 512, fs) != NULL) {
		*shaderSourceRef += buf;
	}

	if(!std::feof(fs)) {
		std::cout<<"ERROR when reading file!";
		return false;
	}
	return true;
}


unsigned int setupShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) {
	unsigned int shaderProgram = glCreateProgram();
	int success;
	char* infoLog;
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout<<"ERROR:SHADER::PROGRAM_LINK::COMPILATION_FAILED: \n%s"<< infoLog;
		return 0;
	}
	return shaderProgram;
}

unsigned int setupShader(GLenum shaderType,const char* shaderSource) {
	unsigned int shader;
	shader = glCreateShader(shaderType);
	std::string shadersource2;
	glShaderSource(shader,1, &shaderSource, NULL);
	glCompileShader(shader);
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		printf("ERROR:SHADER:%s:COMPILATION_FAILED:\n%s\n", shaderName[shaderType], infoLog);
		return 0;
	}
	return shader;
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

	float vertices[] = {
	    // positions         // colors
	     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
	    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
	     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
	};    
	
	unsigned int indices[] = {  // note that we start from 0!
	    0, 1, 2,   // first triangle
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);

	// Similarly, set the attributes of the second index, which are the colors, and enable them.
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float),(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
}

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window) {
	if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, 1);
	}
}


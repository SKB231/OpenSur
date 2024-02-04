#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>


// Function defintions
void frameBufferSizeCallback(GLFWwindow* window,int width,int height);
void processInput(GLFWwindow* window);

// triangle vertices:
float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f, 0.5f, 0.0f
};

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
		

	// Generate a buffer and assign the ID to VBO: Vertex Buffer Object.
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	
	// Bind the GL_ARRAY_BUFFER with the VBO Buffer.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Copies vertex data to buffer.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	const char *vertexShaderSource = "#version 330 core\nlayout (location = 0) in vec3 aPos;\nvoid main()\n{\n   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n}\0";

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader,1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("ERROR:SHADER::VERTEX::COMPILATION_FAILED: \n%s", infoLog);
	}
		

	// Fragment Shader source code:
	const char* fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"\n"
	"void main()\n"
	"{\n"
	"    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\n";

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("ERROR:SHADER::FRAGMENT::COMPILATION_FAILED: \n%s", infoLog);
	}
	
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
		printf("ERROR:SHADER::PROGRAM_LINK::COMPILATION_FAILED: \n%s", infoLog);
	}
	else {
		glUseProgram(shaderProgram);
	}
	// Delete individual shaders since attachment is complete
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	
	// Setup the vertex objects for our VBOs
	// first_arg: Position we are assigning. We set it to 0 since we are assigning the aPos which is the 0th attribute.
	// second_arg: How many components per vertex attribute.
	// third_argument: What type of data are we sending? GL_FLOAT
	// 4th arg: Is our data normalized? No
	// 5th arg: 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float),(void*)0);


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


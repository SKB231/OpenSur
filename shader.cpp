#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "shader.h"
#include <unordered_map>
// Globals
#ifndef GLOBALS
#define GLOBALS
std::unordered_map<GLenum, const char*> shaderName = {
	{GL_VERTEX_SHADER, "VERTEX_SHADER"},
	{GL_FRAGMENT_SHADER, "FRAGMENT_SHADER"}, 
};

#endif // !GLOBALS

Shader :: Shader(const char* vertexPath, const char* fragmentPath) {
	std::string vertexShaderSource, fragmentShaderSrc;

	// Vertex Shader
	bool notHadError = getShaderSource(&vertexShaderSource,"vertexShader.glsl");
	if(!notHadError) {
		return;
	}
	unsigned int vertexShader = setupShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());
	// Fragment Shader
	notHadError = getShaderSource(&fragmentShaderSrc,"fragmentShader.glsl");
	if(!notHadError) {
		return;
	}
	unsigned int fragmentShader = setupShader(GL_FRAGMENT_SHADER, fragmentShaderSrc.c_str());
	if(!vertexShader || !fragmentShader) {
		return;
	}

	unsigned int shaderProgram = setupShaderProgram(vertexShader,fragmentShader);
	if(!shaderProgram) {
		return;
	}
	ID = shaderProgram;
	
	// Clean the shader code
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}


void Shader:: use() {
	glUseProgram(ID);
}


void Shader::setBool(const std::string &name, bool value) const
{         
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
}
void Shader::setInt(const std::string &name, int value) const
{ 
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
}
void Shader::setFloat(const std::string &name, float value) const
{ 
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
} 

void Shader::setVec4(const std::string &name, std::vector<float> value) const
{ 
	if(value.size() < 3) {
		std::cout << "Cannot assign to a unifrom vec4f using a vector only containing 3 values" << std::endl;
	}
		
	glUniform4f(glGetUniformLocation(ID, name.c_str()), value[0], value[1], value[2], value[3]); 
} 


bool Shader :: getShaderSource(std::string* shaderSourceRef, const char* filename) {
	std::ifstream shaderCodeFile;
	// These are differnet error bits in the format of one-hot encoding. So we can safely use the OR operation here.
	shaderCodeFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		shaderCodeFile.open(filename);
		std::stringstream shaderCodeFileStream;
		shaderCodeFileStream << shaderCodeFile.rdbuf();
		shaderCodeFile.close();
		*shaderSourceRef = shaderCodeFileStream.str();
		return true;
	} catch(std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSULLY_READ" << std::endl;
		std::cout << e.what() << "\nError Code: " << e.code()<< std::endl;
		return false;
	}

}



unsigned int Shader:: setupShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) {
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

unsigned int Shader:: setupShader(GLenum shaderType,const char* shaderSource) {
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

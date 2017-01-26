#include "Shader.h"
#include <iostream>

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath) {
	//1. Get the vertex and fragment shader code from file path
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::stringstream vShaderStream;
	std::stringstream fShaderStream;
	const GLchar* vShaderCode;
	const GLchar* fShaderCode;

	//Ensure that files can throw exceptions
	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);

	try {
		//Open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::cout << "Files are open" << std::endl;

		//Read buffer contents into the stringstream
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		std::cout << "Files are read" << std::endl;
		vShaderFile.close();
		fShaderFile.close();
		std::cout << "Files are closed" << std::endl;
		//Convert stream into GLchar array
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		std::cout << "Convert to GLchar array" << std::endl;


	}
	catch (std::ifstream::failure e) {
		std::cout << "Shader file was not read successfully." << std::endl;
	}

	//Convert strings to GLchar
	vShaderCode = vertexCode.c_str();
	fShaderCode = fragmentCode.c_str();
	std::cout << "Convert to GLchar" << std::endl;

	//Compile and link shaders as in source.cpp
	GLuint vertex;
	GLuint fragment;
	GLint success;
	GLchar infoLog[512];

	/*Vertex Shader*/
	vertex = glCreateShader(GL_VERTEX_SHADER);
	std::cout << "glCreateShader" << std::endl;
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// Print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "Vertex Shader failed to compile.\n" << infoLog << std::endl;
	};

	/*Fragment Shader*/
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// Print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "Fragment Shader failed to compile.\n" << infoLog << std::endl;
	}

	/*Linking Shaders*/
	this->Program = glCreateProgram();
	glAttachShader(this->Program, vertex);
	glAttachShader(this->Program, fragment);
	glLinkProgram(this->Program);
	// Print linking errors if any
	glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
		std::cout << "Shader linking failed.\n" << infoLog << std::endl;
	}

	//Once shaders are linked to the program, we can delete them 
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use() {
	//Use the program created in the constructor
	glUseProgram(this->Program);
}

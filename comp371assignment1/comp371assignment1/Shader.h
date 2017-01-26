#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glew.h>; // Include glew to get all the required OpenGL headers

class Shader {
public:
	GLuint Program; //Program ID
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);		// Constructor reads and builds the shader
	void Use();		// Use the program									//Can store fragment and vertex shaders as text files
};

#endif
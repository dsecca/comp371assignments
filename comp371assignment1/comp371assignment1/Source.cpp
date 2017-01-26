// GLEW
#define GLEW_STATIC
#include <glew.h>
// GLFW
#include <glfw3.h>
#include <iostream>
#include "glm\glm\glm.hpp"
#include "glm\glm\gtc\matrix_transform.hpp"
#include "glm\glm\gtc\type_ptr.hpp"
#include "Shader.h"

//Here we check if the escape key is pressed
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a user presses the escape key, we set the WindowShouldClose property to true, 
	// closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
		
}


int main() {

	glfwInit();//Initializes the GLFW library

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//Configures the window (3 indicates version of OpenGL in this case 3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	//Pointer to window object
	GLFWwindow* window = glfwCreateWindow(800, 600, "Assignment 1", nullptr, nullptr);


	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	/************WINDOW HANDLING***************/
	int width;
	int height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	Shader shader("vshader.txt", "fragshader.txt");

	/*For triangle generation*/
	GLfloat vertices[]{ //3D triangle coordinates (x,y,z) here z is zero so triangle looks like it's 2D
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f
	};

	//Can encapsulate this section in a class?
	GLuint VBO, VAO;//A place to store vertices
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);//Giving the buffer above an id
	
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);//Bind the buffer to a target object
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//Copies defined vertex data into buffer's memory
																			  //Linking Vertex Attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//This loop allows the window display to stay open till it is told to stop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents(); //Checks for mouse clicks, keyboard inputs and then calls corresponding functions

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//Clear colour buffer
		glClear(GL_COLOR_BUFFER_BIT);


		/*rendering commands are placed here*/
		shader.use();
		//Apply transformations here

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwSetKeyCallback(window, key_callback);//This function will close the window on the command of the key_callback user function (See above main)
	}
	//Deallocate resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();//Clean up after using resources

	return 0;
}

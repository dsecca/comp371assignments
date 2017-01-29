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

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

glm::vec3 triangle_scale = glm::vec3(1.0f); //shorthand, initializes all 4 components to 1.0f;
glm::vec3 camera_translation = glm::vec3(0.0f, 0.0f, -1.0f);
//glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 3.0f);
//glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);

const float TRIANGLE_MOVEMENT_STEP = 0.1f;
const float CAMERA_PAN_STEP = 0.2f;

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		triangle_scale.x -= TRIANGLE_MOVEMENT_STEP;
	}

	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		triangle_scale.x += TRIANGLE_MOVEMENT_STEP;
	}

	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		triangle_scale.y += TRIANGLE_MOVEMENT_STEP;
	}
		

	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		triangle_scale.y -= TRIANGLE_MOVEMENT_STEP;
	}
		
	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		camera_translation.x += CAMERA_PAN_STEP;
	}
		

	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		camera_translation.x -= CAMERA_PAN_STEP;
	}
		
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		camera_translation.y -= CAMERA_PAN_STEP;
	}

	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		camera_translation.y += CAMERA_PAN_STEP;
	}
		

}

// The MAIN function, from here we start the application and run the game loop
int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Triangle", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);

	// Build and compile our shader programs
	Shader shader("vshader.txt", "fragshader.txt");

	shader.use();

	GLfloat vertices[] = {
		0.0f, 0.5f, 0.0f,  // Top
		0.5f, -0.5f, 0.0f,  // Bottom Right
		-0.5f, -0.5f, 0.0f,  // Bottom Left
	};

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	GLuint transformLoc = glGetUniformLocation(shader.Program, "model_matrix");
	GLuint transformView = glGetUniformLocation(shader.Program, "view_matrix");
	GLuint projectionLoc = glGetUniformLocation(shader.Program, "projection_matrix");

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//camera moevment
		//These we initialized above but since we want it to change at each frame
		//we recall them at each frame generation
		
		/*Camera commands go here*/
		
		glm::mat4 model_matrix;
		model_matrix = glm::scale(model_matrix, triangle_scale);
		//model_matrix = glm::translate(model_matrix, triangle_scale);

		glm::mat4 view_matrix;
		//view_matrix = glm::translate(view_matrix, camera_translation);
		view_matrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), //camera positioned here
			glm::vec3(0.0f, 0.0f, 0.0f), //looks at origin
			glm::vec3(0.0f, 1.0f, 0.0f)); //up vector

		glm::mat4 projection_matrix;
		projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.0f, 100.0f);

		//Here we get the matrices located at the uniform location in the shader programs
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix)); //broadcast the uniform value to the shaders
		glUniformMatrix4fv(transformView, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

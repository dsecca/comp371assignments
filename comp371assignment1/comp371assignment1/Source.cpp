
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
#include <vector>

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;

glm::vec3 triangle_scale = glm::vec3(1.0f); //shorthand, initializes all 4 components to 1.0f;
glm::vec3 triangle_rotate = glm::vec3(1.0f);

 /*Vectors for Points and Translations*/
std::vector<glm::vec3> profilePoints;
std::vector<glm::vec3> trajectoryPoints;
std::vector<GLfloat> vertices;
std::vector<GLuint> indices;
int spans; //For rotation

/*Camera*/
//we give the camera its own coordinate system
glm::vec3 camera_translation = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_position = glm::vec3(0.5f, 0.5f, 3.0f); //z-axis is going through the screen (+ve towards you)
glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f); //Where we want the camera to look, in this case it's the origin

//Camera's z-axis
glm::vec3 camera_direction = glm::normalize(camera_position - camera_target); //Subtracting position and target vectors yields the direction vector

																			  //Camera's x-axis (cross of direction vector (+ve z-axis) and a vector pointing up (in +ve y-axis direction) 
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);// a vector pointing in positive y-axis
glm::vec3 camera_right = glm::normalize(glm::cross(up, camera_direction));

//Camera's y-axis (cross product of camera_direction and camera_right vectors)
glm::vec3 camera_up = glm::cross(camera_direction, camera_right);

const float TRIANGLE_MOVEMENT_STEP = 0.1f;
const float CAMERA_PAN_STEP = 0.2f;
bool keys[1024]; //for camera smootheness
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame
GLfloat lastX = 400; //For mouse callback calculations
GLfloat lastY = 300;
GLfloat yaw = -90.0f;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat pitch = 0.0f;
bool firstMouse = true;
bool wireFrame = true;
GLfloat field_of_view = 45.0f;
GLenum drawingMode = GL_TRIANGLES;//default rendering mode

//MVP
glm::mat4 model_matrix;
glm::mat4 view_matrix;
glm::mat4 projection_matrix;

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	GLfloat pan_speed = 20.0f * deltaTime; //regulate how fast we can pan the camera
	GLfloat scale_speed = 70.0f * deltaTime;
	GLfloat cameraSpeed = 20.0f * deltaTime;

	if (action == GLFW_PRESS) {
		keys[key] = true;
	}
	else if (action == GLFW_RELEASE) {
		keys[key] = false;
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		if (wireFrame == true) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			wireFrame = false;
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			wireFrame = true;
		}

	}

	if (keys[GLFW_KEY_P]) {
		drawingMode = GL_POINTS; //Draw points
	}

	if (keys[GLFW_KEY_W]) {
		drawingMode = GL_LINES; //draw lines
	}

	if (keys[GLFW_KEY_T]) {
		drawingMode = GL_TRIANGLES; //Draw triangles
	}

	if (keys[GLFW_KEY_LEFT]) {
		model_matrix = glm::rotate(model_matrix, cameraSpeed, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	if (keys[GLFW_KEY_RIGHT]) {
		model_matrix = glm::rotate(model_matrix, cameraSpeed, glm::vec3(0.0f, 0.0f, -1.0f));
	}

	if (keys[GLFW_KEY_UP]) {
		model_matrix = glm::rotate(model_matrix, cameraSpeed, glm::vec3(1.0f, 0.0f, 0.0f));
	}

	if (keys[GLFW_KEY_DOWN]) {
		model_matrix = glm::rotate(model_matrix, cameraSpeed, glm::vec3(-1.0f, 0.0f, 0.0f));
	}

	if (keys[GLFW_KEY_L]) {
		camera_position += glm::normalize(glm::cross(camera_direction, camera_up)) * cameraSpeed;
	}


	if (keys[GLFW_KEY_J]) {
		camera_position -= glm::normalize(glm::cross(camera_direction, camera_up)) * cameraSpeed;
	}

	if (keys[GLFW_KEY_K]) {
		camera_position += cameraSpeed * camera_direction;
	}

	if (keys[GLFW_KEY_I]) {
		camera_position -= cameraSpeed * camera_direction;
	}


}
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

	//Prevents screen from jumping when mouse first enters the window
	if (firstMouse) { // this bool variable is initially set to true
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	GLfloat sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	//Constrain the pitch to prevent the screen from flipping
	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	if (pitch < -89.0f) {
		pitch = -89.0f;
	}

	//here we calculate the actual direction vector that results from the adjusted yaw and pitch
	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	camera_translation = glm::normalize(front); //This line may cause errors
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

	GLfloat mouse_scale_speed = 70.0f * deltaTime;
	if (field_of_view >= 1.0f && field_of_view <= 45.0f) {
		field_of_view -= yoffset * mouse_scale_speed;
	}

	if (field_of_view <= 1.0f) {
		field_of_view = 1.0f;
	}

	if (field_of_view >= 45.0f) {
		field_of_view = 45.0f;
	}

}

//Pass input file to determine if rotation or translation
int mode(std::string input) {
	std::fstream file;
	int mode;
	file.open(input);
	file >> mode;//Read first line
	file.close();

	return mode;
}

void loadTranslationSweepData(std::string input) {
	std::fstream file;
	int temp, profile_points, trajectory_points;
	GLfloat x, y, z;
	file.open(input);

	file >> temp; //Skip first line
	file >> profile_points;

	for (int i = 0; i < profile_points; i++) {
		file >> x >> y >> z; //Read points from file
		profilePoints.push_back(glm::vec3(x,y,z));
	}

	file >> trajectory_points;
	for (int i = 0; i < trajectory_points; i++) {
		file >> x >> y >> z; //Read points from file
		trajectoryPoints.push_back(glm::vec3(x, y, z));
	}

	file.close();
}

void loadRotationSweepData(std::string input) {
	std::fstream file;
	int temp, profile_points;
	GLfloat x, y, z = 0.0f;
	file.open(input);

	file >> temp; //Skip first line
	file >> spans;
	file >> profile_points;

	for (int i = 0; i < profile_points; i++) {
		file >> x >> y >> z; //Read points from file
		profilePoints.push_back(glm::vec3(x, y, z));
	}

	file.close();
}

void translateProfile() {
	//May have to order points??
	std::vector<glm::vec3> points_to_translate;
	//Load first profile points into vertices vector
	for (int a = 0; a < profilePoints.size(); a++) {
		//Load first profile points into vector for translations
		points_to_translate.push_back(profilePoints[a]);
		
		//Add first points to vertices vector
		vertices.push_back(profilePoints.at(a).x);
		vertices.push_back(profilePoints.at(a).y);
		vertices.push_back(profilePoints.at(a).z);
	}
	//Get the trajectory points
	for (int i = 0; i < (trajectoryPoints.size()-1); i++) {

		//Calculate the trajectory vector by subtracting one point from another
		glm::vec3 translationVector = trajectoryPoints[i + 1] - trajectoryPoints[i];

		//Add the trajectory points to the previously translated points
		for (int j = 0; j < points_to_translate.size(); j++) {
			//Use the previous point to translate to find the new point
			//Insert the newly translated point at the old point's position
			GLfloat x, y, z;
			x = points_to_translate[j].x + translationVector.x;
			y = points_to_translate[j].y + translationVector.y;
			z = points_to_translate[j].z + translationVector.z;

			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
			
			//Replace the old point to translate with the newly translated point
			points_to_translate.at(j) = glm::vec3(x,y,z);


		}
		
	}

	//Create indices

	for (int i = 0; i < ((vertices.size()/3) - profilePoints.size()); i++){
		if ((i + 1) % profilePoints.size() != 0) {
			// Bottom triangle
			indices.push_back(i);
			indices.push_back(i + profilePoints.size());
			indices.push_back(i + 1);

			// Top triangle
			indices.push_back(i + profilePoints.size());
			indices.push_back(i + 1);
			indices.push_back(i + profilePoints.size() + 1);
		}

	}

}

void rotateProfile() {
	std::vector<glm::vec3> points_to_rotate;
	glm::mat4 rotation;

	for (int a = 0; a < profilePoints.size(); a++) {
		//Load first profile points into vector for translations
		points_to_rotate.push_back(profilePoints[a]);

		//Add first points to vertices vector
		vertices.push_back(profilePoints.at(a).x);
		vertices.push_back(profilePoints.at(a).y);
		vertices.push_back(profilePoints.at(a).z);
	}

	//For each point in points_to_rotate, 
	//we apply a rotation to that point
	//Store the newly rotated point in the vertices vector
	for (int i = 0; i < spans; i++) {

		//Define the rotation matrix to rotate around the z-axis
		rotation = glm::rotate(rotation, (float)(360 / spans), glm::vec3(0.0f, 0.0f, 1.0f));

		for(int j = 0; j < points_to_rotate.size(); j++) {
			
			glm::vec4 point = glm::vec4(points_to_rotate[j],1.0f);//Get the point to rotate in homogenious coordinates

			point = rotation*point;//Apply rotation to point

			//Push each newly rotated coordinate into vertices vector
			vertices.push_back(point.x);
			vertices.push_back(point.y);
			vertices.push_back(point.z);

		}

	}

	//Create indices
	for (int i = 0; i < ((vertices.size() / 3) - profilePoints.size()); i++) {
		if ((i + 1) % profilePoints.size() != 0) {
			// Bottom triangle
			indices.push_back(i);
			indices.push_back(i + profilePoints.size());
			indices.push_back(i + 1);

			// Top triangle
			indices.push_back(i + profilePoints.size());
			indices.push_back(i + 1);
			indices.push_back(i + profilePoints.size() + 1);
		}

	}

}





// The MAIN function, from here we start the application and run the game loop
int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	std::cout << "The file reads " << mode("input_a1.txt") << std::endl;
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Assignment 1", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//Set cursor options to move around with the mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

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

	/**Load data from file into vectors of profile and/or trajectory points**/
	if (mode("input_a1.txt") == 0) {
		loadTranslationSweepData("input_a1.txt");
		translateProfile();
	}
	else {
		loadRotationSweepData("input_a1.txt");
		rotateProfile();
	}


	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	/*Bind first VBO for Profile Curve*/
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()* sizeof(GLfloat), &vertices.front(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices.front(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	GLuint transformLoc = glGetUniformLocation(shader.Program, "model_matrix");
	GLuint transformView = glGetUniformLocation(shader.Program, "view_matrix");
	GLuint projectionLoc = glGetUniformLocation(shader.Program, "projection_matrix");

	/****Game Loop****/
	//Anything that may change at every frame must be declared within the game loop
	while (!glfwWindowShouldClose(window))
	{
		//calculate delta time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		//move();

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// check OpenGL error
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cerr << "OpenGL error: " << err << std::endl;
		}

		view_matrix;
		view_matrix = glm::lookAt(camera_position, camera_position + camera_translation, camera_up);

		projection_matrix;
		projection_matrix = glm::perspective(field_of_view, (GLfloat)width / (GLfloat)height, 0.0f, 100.0f);

		//Here we get the matrices located at the uniform location in the shader programs
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix)); //broadcast the uniform value to the shaders
		glUniformMatrix4fv(transformView, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
		glBindVertexArray(VAO);
		glDrawElements(drawingMode, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}

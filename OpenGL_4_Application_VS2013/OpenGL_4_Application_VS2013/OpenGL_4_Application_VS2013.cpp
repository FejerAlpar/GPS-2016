//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC

#include <iostream>
#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include "Shader.hpp"
#include "Camera.hpp"
#define TINYOBJLOADER_IMPLEMENTATION

#include "Model3D.hpp"
#include "Mesh.hpp"

int glWindowWidth = 640;
int glWindowHeight = 480;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

glm::mat4 model;
glm::mat4 model2;
glm::mat4 model3;
glm::mat4 model4;
glm::mat4 model5;
glm::mat4 model6;
glm::mat4 model7;
GLint modelLoc;
GLint modelLoc2;
GLint modelLoc3;
GLint modelLoc4;
GLint modelLoc5;
GLint modelLoc6;
GLint modelLoc7;
GLint texture;
GLint texture2;
GLint texture3;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;
glm::vec4 lightPosEye;
glm::vec3 viewDirN;
GLuint viewDirNLoc;

GLuint diffuseMap;


gps::Camera myCamera(glm::vec3(0.0f, 0.0f, 2.5f), glm::vec3(0.0f, 0.0f, -10.0f));
float cameraSpeed = 0.1f;
float cameraAngle = 0.0f;
float cameraAngle2 = 0.0f;
float cameraAngle3 = 0.0f;
int delta = 0.0f;

bool pressedKeys[1024];
float angle = 0.0f;
float pitch = 0.0f;
float posz = 0.0f;
float posx = 0.0f;
float posznew = 0.0f;
float posxnew = 0.0f;
float step = 0.2f;
float curve = 0.02f;
int wireframe = 0;
float red = 1.0f;
float green = 1.0f;
float blue = 1.0f;

gps::Model3D myModel;
gps::Model3D myModel2;
gps::Model3D myModel3;
gps::Model3D myModel4;
gps::Model3D myModel5;
gps::Model3D myModel6;
gps::Model3D myModel7;
gps::Shader myCustomShader;

GLuint ReadTextureFromFile(const char* file_name) {
	int x, y, n;
	int force_channels = 4;
	unsigned char* image_data = stbi_load(file_name, &x, &y, &n, force_channels);
	if (!image_data) {
		fprintf(stderr, "ERROR: could not load %s\n", file_name);
		return false;
	}
	// NPOT check
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
		fprintf(
			stderr, "WARNING: texture %s is not power-of-2 dimensions\n", file_name
			);
	}

	int width_in_bytes = x * 4;
	unsigned char *top = NULL;
	unsigned char *bottom = NULL;
	unsigned char temp = 0;
	int half_height = y / 2;

	for (int row = 0; row < half_height; row++) {
		top = image_data + row * width_in_bytes;
		bottom = image_data + (y - row - 1) * width_in_bytes;
		for (int col = 0; col < width_in_bytes; col++) {
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_SRGB, //GL_SRGB,//GL_RGBA,
		x,
		y,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		image_data
		);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	//set projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set Viewport transform
	glViewport(0, 0, retina_width, retina_height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{

}

void processMovement()
{

	if (pressedKeys[GLFW_KEY_R]) {
		red -= 0.1f;
	}

	if (pressedKeys[GLFW_KEY_G]) {
		green -= 0.1f;
	}

	if (pressedKeys[GLFW_KEY_B]) {
		blue -= 0.1f;
	}

	if (pressedKeys[GLFW_KEY_D]) {
		angle -= curve;
	}

	if (pressedKeys[GLFW_KEY_A]) {
		angle += curve;
	}
	if (pressedKeys[GLFW_KEY_S]) {
		posz = posznew;
		posx = posxnew;
		if (angle < 3.1415f / 2) {
			posznew += step * (3.1415f / 2 - angle) / 4;
			posxnew -= step * angle / 4;
		}
		else if (angle >= 3 / 2 * 3.1415f) {
			posznew += step * (angle - 3 / 2 * 3.1415f) / 4;
			posxnew += step * (2 * 3.1415f - angle) / 4;
		}
		else if ((angle >= 3.1415f / 2) && (angle <= 3.1415f)) {
			posznew -= step;
			posxnew -= step * angle / 4;
		}
		else {
			posznew -= step;
			posxnew += step * angle / 4;
		}
	}

	if (pressedKeys[GLFW_KEY_W]) {
		posz = posznew;
		posx = posxnew;
		if (angle < 3.1415f / 2) {
			posznew -= step * (3.1415f / 2 - angle) / 4;
			posxnew -= step * angle / 4;
		}
		else if (angle >= 3 / 2 * 3.1415f) {
			posznew -= step * (angle - 3 / 2 * 3.1415f) / 4;
			posxnew += step * (2 * 3.1415f - angle) / 4;
		}
		else if ((angle >= 3.1415f / 2) && (angle <= 3.1415f)) {
			posznew += step * (3.1415f / 2 - angle) / 8;
			posxnew -= step * angle / 8;
		}
		else {
			posznew += step;
			posxnew += step * angle / 4;
		}
	}

	if (pressedKeys[GLFW_KEY_DOWN]) {
		myCamera.move(gps::MOVE_BACKWARD, 2 * cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_LEFT]) {
		myCamera.move(gps::MOVE_LEFT, 2 * cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_RIGHT]) {
		myCamera.move(gps::MOVE_RIGHT, 2 * cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_UP]) {
		myCamera.move(gps::MOVE_FORWARD, 2 * cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_Q]) {
		cameraAngle -= cameraSpeed;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		cameraAngle += cameraSpeed;
	}

	if (pressedKeys[GLFW_KEY_Z]) {
		cameraAngle2 -= cameraSpeed;
	}

	if (pressedKeys[GLFW_KEY_C]) {
		cameraAngle2 += cameraSpeed;
	}

	if (pressedKeys[GLFW_KEY_T]) {
		cameraAngle3 -= cameraSpeed;
	}

	if (pressedKeys[GLFW_KEY_Y]) {
		cameraAngle3 += cameraSpeed;
	}

	if (pressedKeys[GLFW_KEY_P]) {
		if (wireframe == 0) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			wireframe = 1;
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			wireframe = 0;
		}
	}

	if (angle >= 2 * 3.1415f) angle = 0.0f;
	if (angle < 0) angle = 2 * 3.1415f;
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	//for Mac OS X
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwMakeContextCurrent(glWindow);

	glfwWindowHint(GLFW_SAMPLES, 4);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	//glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels()
{

}

void initShaders()
{
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
}

void initUniforms()
{
	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "diffuseTexture"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(1.0f, 1.0f, 1.0f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//lightDirN ...
	viewDirN = glm::vec3(1.0f);
	viewDirNLoc = glGetUniformLocation(myCustomShader.shaderProgram, "viewDirN");
	glUniform3fv(viewDirNLoc, 1, glm::value_ptr(viewDirN));

	//set light color
	lightColor = glm::vec3(red, green, blue); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	//light posEye
	lightPosEye = glm::vec4(1.0f);

}

void renderScene()
{
	delta += 0.001f;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.8, 0.8, 0.8, 1.0);
	glViewport(0, 0, retina_width, retina_height);

	processMovement();
	myCustomShader.useShaderProgram();

	//initialize the model matrix
	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	// car movement
	model = glm::translate(model, glm::vec3(0, 0, posznew));
	model = glm::translate(model, glm::vec3(posxnew, 0, 0));
	model = glm::rotate(model, angle, glm::vec3(0, 1, 0));


	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "diffuseTexture"), 0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	myModel.Draw(myCustomShader);


	model2 = glm::mat4(1.0f);
	modelLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	model2 = glm::translate(model2, glm::vec3(0, -0.6, 0));
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "diffuseTexture"), 0);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model2));
	myModel2.Draw(myCustomShader);

	model3 = glm::mat4(1.0f);
	modelLoc3 = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	model3 = glm::translate(model3, glm::vec3(0, -0.6, -93));
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "diffuseTexture"), 0);
	glBindTexture(GL_TEXTURE_2D, texture3);
	glUniformMatrix4fv(modelLoc3, 1, GL_FALSE, glm::value_ptr(model3));
	myModel3.Draw(myCustomShader);
	
	model4 = glm::mat4(1.0f);
	modelLoc4 = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	model4 = glm::translate(model4, glm::vec3(-95, -0.6, -93));
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "diffuseTexture"), 0);
	glBindTexture(GL_TEXTURE_2D, texture3);
	glUniformMatrix4fv(modelLoc4, 1, GL_FALSE, glm::value_ptr(model4));
	myModel4.Draw(myCustomShader);
	
	model5 = glm::mat4(1.0f);
	modelLoc5 = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	model5 = glm::translate(model5, glm::vec3(-95, -0.6, 0));
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "diffuseTexture"), 0);
	glBindTexture(GL_TEXTURE_2D, texture3);
	glUniformMatrix4fv(modelLoc5, 1, GL_FALSE, glm::value_ptr(model5));
	myModel5.Draw(myCustomShader);


	myModel6.Draw(myCustomShader);
	model6 = glm::mat4(1.0f);
	modelLoc6 = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	model6 = glm::translate(model5, glm::vec3(95, -0.6, 0));
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "diffuseTexture"), 0);
	glBindTexture(GL_TEXTURE_2D, texture3);
	glUniformMatrix4fv(modelLoc5, 1, GL_FALSE, glm::value_ptr(model6));
	myModel6.Draw(myCustomShader);
	
	myModel7.Draw(myCustomShader);
	model7 = glm::mat4(1.0f);
	modelLoc7 = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	model7 = glm::translate(model5, glm::vec3(0, -0.6, 93));
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "diffuseTexture"), 0);
	glBindTexture(GL_TEXTURE_2D, texture3);
	glUniformMatrix4fv(modelLoc5, 1, GL_FALSE, glm::value_ptr(model7));
	myModel7.Draw(myCustomShader);


	//initialize the view matrix
	glm::mat4 view = myCamera.getViewMatrix();
	
	//camera rotations
	view = glm::rotate(view, cameraAngle, glm::vec3(0, 1, 0));
	view = glm::rotate(view, cameraAngle2, glm::vec3(1, 0, 0));
	view = glm::rotate(view, cameraAngle3, glm::vec3(0, 0, 1));
	view = glm::translate(view, glm::vec3(0, -10, 0));

	//send matrix data to shader
	GLint viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));


	//set light color
	lightColor = glm::vec3(red, green, blue); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
}

int main(int argc, const char * argv[]) {

	initOpenGLWindow();
	initOpenGLState();
	initShaders();
	myModel = gps::Model3D("objects/Porsche_911_GT2.obj", "objects");
	texture = ReadTextureFromFile("textures/0000.BMP");

	myModel2 = gps::Model3D("objects/Street environment_V01.obj", "objects");
	myModel3 = gps::Model3D("objects/Street environment_V01.obj", "objects");
	myModel4 = gps::Model3D("objects/Street environment_V01.obj", "objects");
	myModel5 = gps::Model3D("objects/Street environment_V01.obj", "objects");
	myModel6 = gps::Model3D("objects/Street environment_V01.obj", "objects");
	myModel7 = gps::Model3D("objects/Street environment_V01.obj", "objects");
	texture2 = ReadTextureFromFile("textures/Building_V02_C.png");
	texture3 = ReadTextureFromFile("textures/Building_V01_C.png");
	initUniforms();

	while (!glfwWindowShouldClose(glWindow)) {
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	//close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}

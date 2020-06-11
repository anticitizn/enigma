#pragma once
#include <string>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glad/glad.h"
#include "SDL.h"
#include "SDL_opengl.h"

using namespace std;

class Camera {
public:
	float yaw = 90.0f;
	float pitch = 0.0f;

	const float moveSpeed = 0.1f;
	const float lookSpeed = 0.5f;

	glm::vec3 front;
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 right;

	glm::vec3 position = glm::vec3(0.0f);

	void processKeyboard(const string direction, const float deltaTime)
	{
		if (direction == "FRONT")
			position += moveSpeed * front;
		if (direction == "BACK")
			position -= moveSpeed * front;
		if (direction == "RIGHT")
			position += moveSpeed * right;
		if (direction == "LEFT")
			position -= moveSpeed * right;
		if (direction == "UP")
			position += moveSpeed * up;
		if (direction == "DOWN")
			position -= moveSpeed * up;

		calculateVectors();
	}
	
	void processMouse(float xrel, float yrel)
	{
		xrel *= lookSpeed;
		yrel *= lookSpeed;

		yaw += xrel;
		pitch -= yrel;
		// ensure that the camera doesn't flip
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		calculateVectors();
	}

	glm::mat4 getViewMatrix() 
	{
		return glm::lookAt(position, position + front, up);
	}

	Camera()
	{
		calculateVectors();
	}

private:

	void calculateVectors()
	{
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(front);
		right = glm::normalize(glm::cross(front, up));
	}
};
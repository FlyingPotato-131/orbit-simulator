#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

const float pi = 3.141592653589793;

struct camera{
	glm::vec3 target;
	float distance;
	float pitch;
	float yaw;
	glm::vec3 up;
};

camera createCamera(glm::vec3 pos, glm::vec3 target, glm::vec3 up = {0.0f, 1.0f, 0.0f}){
	return{
		target,
		glm::length(pos - target),
		asin((pos.y - target.y) / glm::length(pos - target)),
		asin((pos.x - target.x) / glm::length(pos - target)),
		up
	};
}

camera rotateCamera(GLFWwindow *window, camera prev, float deltaTime){
	const float rotationSpeed = 0.5f * deltaTime;
	const float zoomSpeed = 0.5f * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && prev.pitch < 0.49 * pi)
		prev.pitch = std::fmod(prev.pitch + rotationSpeed, 2 * pi);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && prev.pitch > -0.49 * pi)
		prev.pitch = std::fmod(prev.pitch - rotationSpeed, 2 * pi);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		prev.yaw = std::fmod(prev.yaw - rotationSpeed, 2 * pi);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		prev.yaw = std::fmod(prev.yaw + rotationSpeed, 2 * pi);
	if(glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
		prev.distance -= zoomSpeed * prev.distance;
	if(glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
		prev.distance += zoomSpeed * prev.distance;
	return prev;
}
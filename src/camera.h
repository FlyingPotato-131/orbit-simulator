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

glm::vec3 cameraPos(camera view){
	return {
		cos(view.pitch) * sin(view.yaw),
		sin(view.pitch),
		cos(view.pitch) * cos(view.yaw)
	};
}

camera createCamera(glm::vec3 pos, glm::vec3 target, glm::vec3 up = {0.0f, 1.0f, 0.0f}){
	return{
		target,
		glm::length(pos - target),
		float(asin((pos.y - target.y) / glm::length(pos - target))),
		float(asin((pos.x - target.x) / glm::length(pos - target))),
		up
	};
}

camera rotateCamera(GLFWwindow *window, camera prev, float deltaTime){
	const float rotationSpeed = 0.05f * deltaTime;
	const float zoomSpeed = 0.05f * deltaTime;

	// if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && prev.pitch < 0.49 * pi)
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		prev.pitch = prev.pitch + rotationSpeed;
		// prev.pitch = std::fmod(prev.pitch + rotationSpeed, 0.5 * pi);
	// if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && prev.pitch > -0.49 * pi)
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		prev.pitch = prev.pitch - rotationSpeed;
		// prev.pitch = std::fmod(prev.pitch - rotationSpeed, 0.5 * pi);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		prev.yaw = std::fmod(prev.yaw - rotationSpeed, 2 * pi);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		prev.yaw = std::fmod(prev.yaw + rotationSpeed, 2 * pi);
	if(glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS && prev.distance > 0.1)
		prev.distance -= zoomSpeed * prev.distance;
	if(glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
		prev.distance += zoomSpeed * prev.distance;

	if(prev.pitch > 0.49 * pi)
		prev.pitch = 0.49 * pi;
	if(prev.pitch < -0.49 * pi)
		prev.pitch = -0.49 * pi;
	// if(std::abs(prev.pitch) > 0.5 * pi)
	// 	prev.up = {0.0f, -1.0f, 0.0f};
	// if(std::abs(prev.pitch) <= 0.5 * pi)
	// 	prev.up =  {0.0f, 1.0f, 0.0f};
	return prev;
}

struct mouseCameraOld{
	camera view;
	float x;
	float y;
};

mouseCameraOld mouseCamera(GLFWwindow* window, float oldx, float oldy, camera prev){
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	const float zoomSpeed = 0.1f;
		
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
		const float sensitivity = 0.01f;
		float dx = x - oldx;
		float dy = y - oldy;
		prev.yaw = std::fmod(prev.yaw - dx * sensitivity, 2 * pi);
		prev.pitch += dy * sensitivity;
		if(prev.pitch > 0.49 * pi)
			prev.pitch = 0.49 * pi;
		if(prev.pitch < -0.49 * pi)
			prev.pitch = -0.49 * pi;
	}

	double *v = reinterpret_cast<double *>(glfwGetWindowUserPointer(window));

	if((*v > 0 && prev.distance > 0.1) || *v < 0){
		prev.distance -= *v * zoomSpeed * prev.distance;
	}
	// prev.distance -= *v * sensitivity;
	*v = 0;

	return {prev, float(x), float(y)};
}
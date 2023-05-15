#pragma once
#include <glm/glm.hpp>

const float G = 0.000000000000066743;
// const float earthMass = 
const float GxM = 398332.4;
const float mass = 42000;
const float dragCoeff = 0.01f;

struct state{
	glm::vec3 pos;
	glm::vec3 v;
};

state movedt(state current, glm::vec3 a, float dt){
	return{
		current.pos + current.v * dt + a * dt * dt,
		current.v + a * dt
	};
}

glm::vec3 gravForce(state current){
	return -mass * glm::normalize(current.pos) * GxM / glm::dot(current.pos, current.pos);
}

glm::vec3 drag(state current){
	return -dragCoeff * current.v * static_cast<float>(exp(-glm::length(current.pos)));
}
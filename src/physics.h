#pragma once
#include <glm/glm.hpp>
#include <vector>

const float G = 0.000000000000066743;
// const float earthMass = 
const float GxM = 398332.4;
const float mass = 42000;
const float dragCoeff = 0.01f;
const float twr = 1;
// const float pi = 3.141592653589793;

struct state{
	glm::vec3 pos;
	glm::vec3 v;
	glm::vec3 rotate;
};

struct orbit{
	float ascN;
	float incl;
	float argP;
	float mAxis;
	float ecc;
	float tAnom;
};

state movedt(state current, glm::vec3 a, float dt){
	return{
		current.pos + current.v * dt + a * dt * dt,
		current.v + a * dt,
		current.rotate
	};
}

glm::vec3 gravForce(state current){
	return -mass * glm::normalize(current.pos) * GxM / glm::dot(current.pos, current.pos);
}

glm::vec3 drag(state current){
	return -dragCoeff * current.v * static_cast<float>(exp(-glm::length(current.pos)));
}

glm::mat3 localCoords(state current){
	glm::vec3 normal = glm::normalize(cross(current.pos, current.v));
	glm::vec3 radial = glm::normalize(cross(current.v, normal));
	return glm::mat3(glm::normalize(current.v), radial, -normal);
	// return glm::mat3(radial, normal, current.v);
}

orbit createOrbit(state current){
	glm::vec3 north = {0.0, 1.0, 0.0};
	glm::vec3 vern = {0.0, 0.0, -1.0};
	glm::vec3 h = cross(current.pos, current.v);
	glm::vec3 n = cross(north, h);
	glm::vec3 e = ((glm::dot(current.v, current.v) - GxM / glm::length(current.pos)) * current.pos - glm::dot(current.pos, current.v) * current.v) / GxM;

	return {
		n.x >= 0 ? 2 * pi - float(acos(glm::dot(vern, glm::normalize(n)))) : float(acos(glm::dot(vern, glm::normalize(n)))),
		pi + float(acos(glm::dot(north, glm::normalize(h)))),
		e.y < 0 ? 2 * pi - float(acos(glm::dot(glm::normalize(n), glm::normalize(e)))) : float(acos(glm::dot(glm::normalize(n), glm::normalize(e)))),
		-GxM / (glm::dot(current.v, current.v) - 2 * GxM / glm::length(current.pos)),
		length(e),
		glm::dot(current.pos, current.v) < 0 ? 2 * pi - float(acos(glm::dot(glm::normalize(e), glm::normalize(current.pos)))) : float(acos(glm::dot(glm::normalize(e), glm::normalize(current.pos))))
	};
}

glm::vec3 posAtT(orbit o, float T){
	float r = o.mAxis * (1 - o.ecc * o.ecc) / (1 - o.ecc * cos(o.tAnom + T));
	return {
		r * (cos(o.argP + o.tAnom + T) * sin(o.ascN) + sin(o.argP + o.tAnom + T) * cos(o.incl) * cos(o.ascN)),
		r * sin(o.argP + o.tAnom + T) * sin(o.incl),
		r * (cos(o.argP + o.tAnom + T) * cos(o.ascN) + sin(o.argP + o.tAnom + T) * cos(o.incl) * sin(o.ascN))
	};
}

std::vector<glm::vec3> orbitMesh(orbit o, unsigned int n){
	float dT = 2 * pi / float(n);
	std::vector<glm::vec3> res;
	for (int i = 0; i < n; ++i){
		float r = o.mAxis * (1 - o.ecc * o.ecc) / (1 - o.ecc * cos(o.tAnom + float(i) * dT));
		if(r > 0)
			res.push_back(posAtT(o, float(i) * dT));
	}
	return res;
}
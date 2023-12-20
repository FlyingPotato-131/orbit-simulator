#pragma once
#include <glm/glm.hpp>
#include <vector>

const float G = 0.0000066743;
// const float earthMass = 
// const float GxM = 398332.4;
const float GxMmoon = 4900.27106;
// const float mass = 42000;
const float mass = 25000000.0;
const float earthMass = 940000000.0;
const float GxmM = 156846050000.0;
const float dragCoeff = 0.01f;
const float twr = 1;
// const float Gxmu = 1.6253476683937824;
const float Gxmu = 6440.7;
// const float pi = 3.141592653589793;

struct state{
	glm::vec3 pos;
	glm::vec3 v;
	glm::vec3 rotate;
};

struct orbit{
	glm::vec3 focus;
	float ascN;
	float incl;
	float argP;
	float mAxis;
	float ecc;
	float tAnom;
};

state movedt(state current, glm::vec3 a, float dt){
	return{
		current.pos + current.v * dt + 0.5f * a * dt * dt,
		current.v + a * dt,
		current.rotate
	};
}

// state movedt(state current, glm::vec3 a, float dt){
// 	return{
// 		current.pos + current.v * dt + 0.5f * a * dt * dt,
// 		current.v + a * dt,
// 		current.rotate
// 	};
// }

glm::vec3 gravForce(state current, glm::vec3 earthPos){
	return -glm::normalize(current.pos - earthPos) * GxmM / glm::dot(current.pos - earthPos, current.pos - earthPos);
}

glm::vec3 moonGrav(state current, glm::vec3 moonPos){
	return -mass * glm::normalize(current.pos - moonPos) * GxMmoon / glm::dot(current.pos, current.pos);
}

glm::vec3 drag(state current){
	return -dragCoeff * current.v * static_cast<float>(exp(-glm::length(current.pos)));
}

glm::vec3 STRacc(state current, glm::vec3 F){
	glm::vec3 Fll = glm::length(F) * glm::dot(glm::normalize(current.v), glm::normalize(F)) * glm::normalize(current.v);
	glm::vec3 Forth = F - Fll;
	glm::vec3 all = float(pow(1 - glm::dot(current.v, current.v) / 89875517873.68175, 1.5) / mass) * Fll;
	glm::vec3 aorth = float(sqrt(1 - glm::dot(current.v, current.v) / 89875517873.68175) / mass) * Forth;
	return all + aorth;
}

glm::vec3 NTacc(state current, glm::vec3 F){
	return F / mass;
}

glm::vec3 acc(state current, glm::vec3 earthPos, glm::vec3 thrust){
	glm::vec3 F = (gravForce(current, earthPos) + thrust * mass);
	return NTacc(current, F);
}

state movedtRK4(state current, float dt, glm::vec3 earthPos, glm::vec3 thrust){
	glm::vec3 kv1 = dt * acc(current, earthPos, thrust);
	glm::vec3 kp1 = dt * current.v;

	glm::vec3 kv2 = dt * acc({current.pos, current.v + 0.5f * kv1, current.rotate}, earthPos, thrust);
	glm::vec3 kp2 = dt * (current.v + 0.5f * kv1);

	glm::vec3 kv3 = dt * acc({current.pos, current.v + 0.5f * kv2, current.rotate}, earthPos, thrust);
	glm::vec3 kp3 = dt * (current.v + 0.5f * kv2);

	glm::vec3 kv4 = dt * acc({current.pos, current.v + kv3, current.rotate}, earthPos, thrust);
	glm::vec3 kp4 = dt * (current.v + kv3);

	return {
		current.pos + 0.166666f * kp1 + 0.33333f * kp2 + 0.33333f * kp3 + 0.166666f * kp4,
		current.v + 0.166666f * kv1 + 0.33333f * kv2 + 0.33333f * kv3 + 0.166666f * kv4,
		current.rotate
	};
}

glm::mat3 localCoords(state current){
	glm::vec3 normal = glm::normalize(cross(current.pos, current.v));
	glm::vec3 radial = glm::normalize(cross(current.v, normal));
	return glm::mat3(glm::normalize(current.v), radial, -normal);
	// return glm::mat3(radial, normal, current.v);
}

// orbit createOrbit(state current, glm::vec3 earthPos){
// 	current.pos = earthPos - current.pos;
// 	glm::vec3 north = {0.0, 0.0, 1.0};
// 	glm::vec3 vern = {1.0, 0.0, 0.0};
// 	glm::vec3 h = cross(current.pos, current.v);
// 	glm::vec3 n = cross(north, h);
// 	glm::vec3 e = ((glm::dot(current.v, current.v) - GxM / glm::length(current.pos)) * current.pos - glm::dot(current.pos, current.v) * current.v) / GxM;

// 	return {
// 		n.y < 0 ? 2 * pi - float(acos(glm::dot(vern, glm::normalize(n)))) : float(acos(glm::dot(vern, glm::normalize(n)))),
// 		float(acos(glm::dot(north, glm::normalize(h)))),
// 		e.z < 0 ? 2 * pi + float(acos(glm::dot(glm::normalize(n), glm::normalize(e)))) : -float(acos(glm::dot(glm::normalize(n), glm::normalize(e)))),
		// -GxM / (glm::dot(current.v, current.v) - 2 * GxM / glm::length(current.pos)),
// 		length(e),
// 		glm::dot(current.pos, current.v) < 0 ? 2 * pi + float(acos(glm::dot(glm::normalize(e), glm::normalize(current.pos)))) : -float(acos(glm::dot(glm::normalize(e), glm::normalize(current.pos))))
// 	};
// }

orbit createOrbit(state current, glm::vec3 earthPos){
	current.pos = current.pos - (current.pos * mass + earthPos * earthMass) / (mass + earthMass);
	glm::vec3 north = {0.0, 1.0, 0.0};
	// glm::vec3 vern = {0.0, 0.0, 1.0};
	glm::vec3 h = cross(current.pos, current.v);
	glm::vec3 n = cross(north, h);
	// glm::vec3 n = h.y;
	// glm::vec3 e = cross(current.v, h) / GxM - glm::normalize(current.pos);
	glm::vec3 e = cross(current.v, h) / Gxmu - current.pos / length(current.pos);

	return {
		earthPos,
		n.x > 0 ? float(acos(n.z / length(n))) : 2 * pi - float(acos(n.z / length(n))),
		float(acos(h.y / length(h))),
		e.y > 0 ? float(acos(glm::dot(n, e) / length(n) / length(e))) : 2 * pi - float(acos(glm::dot(n, e) / length(n) / length(e))),
		-Gxmu / (glm::dot(current.v, current.v) - 2 * Gxmu / length(current.pos)),
		length(e),
		glm::dot(current.pos, current.v) > 0 ? float(acos(glm::dot(e, current.pos) / length(e) / length(current.pos))) : 2 * pi - float(acos(glm::dot(e, current.pos) / length(e) / length(current.pos)))
	};
}

glm::vec3 posAtT(orbit o, float T){
	// T = -T;
	float r = o.mAxis * (1 - o.ecc * o.ecc) / (1 + o.ecc * cos(o.tAnom + T));
	return o.focus + glm::vec3{
		r * (cos(o.argP + o.tAnom + T) * sin(o.ascN) + sin(o.argP + o.tAnom + T) * cos(o.incl) * cos(o.ascN)),
		r * sin(o.argP + o.tAnom + T) * sin(o.incl),
		r * (cos(o.argP + o.tAnom + T) * cos(o.ascN) - sin(o.argP + o.tAnom + T) * cos(o.incl) * sin(o.ascN)),
	};
}

// glm::vec3 posAtT(orbit o, float T){

// }

std::vector<glm::vec3> orbitMesh(orbit o, unsigned int n, glm::vec3 shift){
	float dT = 2 * pi / float(n);
	std::vector<glm::vec3> res;
	for (int i = 0; i < n; ++i){
		float r = o.mAxis * (1 - o.ecc * o.ecc) / (1 + o.ecc * cos(o.tAnom + float(i) * dT));
		if(r > 0)
			res.push_back(posAtT(o, float(i) * dT) + shift);
	}
	return res;
}
#version 330 core

out vec4 FragColor;

in vec3 fragPos;

uniform vec3 cameraPos;
uniform vec3 earthPos;

void main(){
	FragColor = vec4(0.49, 0.5, 0.81, 1.0);

	vec3 viewDir = fragPos - cameraPos;
	float D = dot(cameraPos - earthPos, viewDir) * dot(cameraPos - earthPos, viewDir) - dot(viewDir, viewDir) * (dot(cameraPos - earthPos, cameraPos - earthPos) - 220618.09);

	if(D > 0 && dot(viewDir, fragPos - earthPos) > 0 || length(fragPos - earthPos) > 200000 || length(fragPos) < 0.4 * sqrt(length(cameraPos)))
		discard;
	// if(length(fragPos) < 0.4 * sqrt(length(cameraPos)))
}
#version 330 core

out vec4 FragColor;

in vec3 fragPos;

uniform vec3 cameraPos;

void main(){
	FragColor = vec4(0.49, 0.5, 0.81, 1.0);

	vec3 viewDir = fragPos - cameraPos;
	float D = dot(cameraPos, viewDir) * dot(cameraPos, viewDir) - dot(viewDir, viewDir) * (dot(cameraPos, cameraPos) - 40960000);

	if(D > 0 && dot(viewDir, fragPos) > 0 || length(fragPos) > 200000 || length(fragPos) < 6400)
		discard;
}
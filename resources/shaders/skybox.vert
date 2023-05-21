#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;
out vec3 screenCoord;

uniform mat4 projection;
uniform mat4 view;
// uniform mat4 model;
uniform vec3 cameraPos;
uniform vec3 target;
uniform vec3 pos;
uniform vec3 velocity;

void main(){
	TexCoords = aPos;
	gl_Position = projection * view * vec4(aPos, 1.0);
	gl_Position = gl_Position.xyww;
	// screenCoord = vec3(gl_Position);
}
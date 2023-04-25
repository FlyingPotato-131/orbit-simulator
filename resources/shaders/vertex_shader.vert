#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

// out vec3 vertexColor;
out vec2 texCoord;

// uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// uniform float xoffset;

void main(){
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    // vertexColor = aPos;
    // vertexColor = aColor;
    texCoord = aTexCoord;
    // vertexColor = vec4(0.5, 0.0, 0.0, 1.0);
}

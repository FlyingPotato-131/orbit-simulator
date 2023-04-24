#version 330 core
layout (location = 0) in vec3 aPos;
void main(){
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}

// #version 330 core

// layout (location=0) in vec3 position;
// layout (location=1) in vec2 texCoord;
// layout (location=2) in vec3 vertexNormal;

// uniform mat4 modelViewMatrix;
// uniform mat4 projectionMatrix;

// void main()
// {
//     vec4 mvPos = modelViewMatrix * vec4(position, 1.0);
//     gl_Position = projectionMatrix * mvPos;
// }
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
    float far = 2000; 
    float near = 1;

    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    
    gl_Position.z = 2.0*log(gl_Position.w/near + 1)/log(far/near + 1) - 1; 
    gl_Position.z *= gl_Position.w;

    texCoord = aTexCoord;
}

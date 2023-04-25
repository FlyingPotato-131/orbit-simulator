// #version 330 core
// layout (points) in;
// layout (points, max_vertices = 1) out;

// void main() {    
//     gl_Position = gl_in[0].gl_Position; 
//     EmitVertex();
//     EndPrimitive();
// }

#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 vBC;

void main()
{
    gl_Position = gl_in[0].gl_Position;
    vBC = vec3(1, 0, 0);
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    vBC = vec3(0, 1, 0);
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    vBC = vec3(0, 0, 1);
    EmitVertex();

    EndPrimitive();
}
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 cameraPos;

out vec3 color;

out vec3 fragPos;
// out vec3 screenPos;

// uniform vec3 cameraPos;
// uniform vec3 target;

void main(){
    // float far = 1000000;
    // float near = 0.01;

	gl_Position = projection * view * vec4(aPos, 1.0);
	gl_Position.z *= 0.9;
	// gl_Position.z = 0.99;

    // gl_Position.z = 2.0*log(gl_Position.w/near + 1)/log(far/near + 1) - 1; 
    // gl_Position.z *= gl_Position.w;

	fragPos = aPos;
    // vec3 screenPos = vec3(gl_Position);

    // vec3 viewDir = aPos - cameraPos;
	// float D = dot(cameraPos, viewDir) * dot(cameraPos, viewDir) - dot(viewDir, viewDir) * (dot(cameraPos, cameraPos) - 40960000);
	// if((D < 0 || dot(viewDir, aPos) < 0))
		// gl_Position.z *= 0.9;
		// gl_Position.z = 2;	
	// else
	// 	gl_Position.z *= 0.5;
	// 	color = vec3(1.0);
	// else
	// 	color = vec3(1.0, 0.0, 0.0);
}
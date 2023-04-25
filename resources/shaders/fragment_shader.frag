#version 330 core
out vec4 FragColor;
// in vec3 vertexColor;
in vec2 texCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixvalue;

// uniform vec4 Color;

void main(){
	// FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
	// FragColor = vec4(vertexColor, 1.0);
	// FragColor = texture(inTexture, texCoord);
	FragColor = mix(texture(texture1, texCoord), texture(texture2, vec2(texCoord.x, 1-texCoord.y)), mixvalue);
}

// #version 330 core

// in vec3 vBC;

// out vec4 fragColor;

// struct Material
// {
// 	vec3 colour;
// 	int useColour;
// 	float reflectance;
// };

// uniform vec3 ambientLight;
// uniform Material material;

// void main()
// {
// 	vec4 baseColour = vec4(material.colour, 1.0);
	
// 	vec4 black = vec4(0, 0, 0, 0);
// 	vec4 white = vec4(1, 1, 1, 1);

// 	if(any(lessThan(vBC, vec3(0.02)))) {
// 		fragColor = white;
// 	}
// 	else {
// 		fragColor = black;
// 	}

// 	vec4 totalLight = vec4(ambientLight, 1.0);
// }
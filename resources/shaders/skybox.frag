#version 330 core

out vec4 FragColor;

in vec3 TexCoords;
// in vec3 FragPos;

uniform samplerCube skybox;
uniform sampler2D earthDay;
uniform vec3 lightDir;
uniform vec3 viewDir;
uniform vec3 up;
uniform vec3 cameraPos;

float signedMod(float x, float y){
	return x - y * round(x / y);
}

void main(){
	float pi = 3.141592653589793;

	vec3 viewRay = TexCoords;
	float D = dot(cameraPos, viewRay) * dot(cameraPos, viewRay) - dot(viewRay, viewRay) * (dot(cameraPos, cameraPos) - 3000 * 3000);
	float t = (-dot(cameraPos, viewRay) - sqrt(abs(D))) / dot(viewRay, viewRay);

	if(D > 0 && t > 0){
		vec3 fragPos = normalize(cameraPos + viewRay * t);	

		float theta = asin(fragPos.y);
	    vec2 earthTex = vec2(-atan(fragPos.z / cos(theta), fragPos.x / cos(theta)), theta);
	    earthTex = vec2(0.5 * earthTex.x / pi + 0.5, earthTex.y / pi + 0.5);

		vec3 color = vec3(texture(earthDay, earthTex));
		// FragColor = vec4(color * dot(fragPos, lightDir), 1.0);
		FragColor = vec4(color, 1.0);
	}else{
		float sunSize = 0.0175;
		// float sunSize = 1;
		float brightness = 5;
		vec3 sunColor = vec3(0.42, 0.39, 0.19);
		// vec3 rayColor = vec3(1.0);
		vec3 rayColor = sunColor;

		float sunAngle = acos(dot(lightDir, normalize(TexCoords)));
		// vec3 sun = (sunAngle <= sunSize) ? brightness * (1 - pow(sunAngle / sunSize, 2)) * sunColor : vec3(0.0);
		// vec3 sun = brightness * sunColor * exp(-pow(sunAngle / sunSize, 1));
		vec3 sun = brightness * sunColor / pow(1 + pow(sunAngle / sunSize, 2), 0.75);

		vec3 right = normalize(cross(up, lightDir));
		vec3 upLight = normalize(cross(lightDir, right));
		vec3 proj = upLight * dot(upLight, TexCoords) + right * dot(right, TexCoords);
		float rayAngle = acos(dot(normalize(proj), upLight));
		int n = 14;
		float flareAngle = dot(lightDir, normalize(TexCoords));
		vec3 ray = flareAngle > 0 ? rayColor * pow(n / pi * abs(signedMod(rayAngle, 2 * pi / n)), 2)
		/ pow(1 + pow(sunAngle / sunSize, 2), 0.5) * pow(abs(dot(viewDir, lightDir)), 16) : vec3(0.0);
		// float flareDistance = 5 * sunSize * pow(n / pi * abs(signedMod(rayAngle, 2 * pi / n)), 2) + 2 * sunSize;
		// vec3 ray = brightness * rayColor * pow(mod(rayAngle, pi/4) - pi/8, 4);
		// ray = ray * pow(flareAngle / 2, 4);
		// vec3 ray = length(proj) < flareDistance ? vec3(1.0) : vec3(0.0);
		// vec3 ray = length(proj) <= flareDistance && flareAngle > 0 ? rayColor * brightness * (1 - pow(length(proj) / flareDistance, 1)) * 
		// pow(n / pi * abs(signedMod(rayAngle, 2 * pi / n)), 8) : vec3(0.0);
		// vec3 ray = vec3(0.0);

		FragColor = texture(skybox, TexCoords) + vec4(sun, 1.0) + vec4(ray, 1.0);
		// FragColor = vec4(sun, 1.0) + vec4(ray, 1.0);
		// FragColor = vec4(sun, 1.0);
		// FragColor = vec4(ray, 1.0);
	}
}
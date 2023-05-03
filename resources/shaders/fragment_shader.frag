#version 330 core

// struct Material{
// 	vec3 ambient;
// 	vec3 diffuse;
// 	vec3 specular;
// 	float shininess;
// };

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

// uniform sampler2D texture_diffuse1;
// uniform sampler2D texture_diffuse2;
// uniform sampler2D texture_diffuse3;
// uniform sampler2D texture_specular1;
// uniform sampler2D texture_specular2;

struct Light{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

uniform Material material;
uniform Light light;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 objectColor;
// uniform vec3 lightColor;
// uniform vec3 lightPos;
uniform vec3 viewPos;

void main(){
	// float shininess = 32.0;
	// Material material;

	// material.diffuse = texture_diffuse1;
	// material.specular = texture_specular1;
	// material.shininess = 32.0;

	float distance = length(light.position - FragPos);
	// float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	float attenuation = 1.0;

	// vec3 ambient = lightColor * material.ambient * light.ambient;
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	// vec3 ambient = vec3(0.0);

	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	// vec3 lightDir = normalize(light.position);
	float diff = max(dot(norm, lightDir), 0.0);
	// vec3 diffuse = lightColor * (diff * material.diffuse) * light.diffuse;
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = dot(reflectDir, norm) > 0 ? pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) : 0;
	// float spec = 1 > 0 ? pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) : 0;
	// vec3 specular = lightColor * (spec * material.specular) * light.specular;
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	vec3 result = attenuation * (diffuse + specular) + ambient;
	FragColor = vec4(result, 1.0);	
}
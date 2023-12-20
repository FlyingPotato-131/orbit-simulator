#version 330 core

// struct Material{
// 	vec3 ambient;
// 	vec3 diffuse;
// 	vec3 specular;
// 	float shininess;
// };

struct Material {
	sampler2D diffuse1;
	sampler2D diffuse2;
	sampler2D surface1;
	sampler2D surface2;
	// sampler2D normalMap;
	sampler2D normal;
	sampler2D lightmap;
	sampler2D emission;
	float shininess;
};

// uniform sampler2D texture_diffuse1;
// uniform sampler2D texture_diffuse2;
// uniform sampler2D texture_diffuse3;
// uniform sampler2D texture_specular1;
// uniform sampler2D texture_specular2;

// struct Light{
// 	vec3 position;

// 	vec3 ambient;
// 	vec3 diffuse;
// 	vec3 specular;

// 	float constant;
// 	float linear;
// 	float quadratic;
// };

uniform Material material;
// uniform Light light;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
// in vec3 lightDir;
// in vec3 viewDir;
in mat3 TBN;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

const float PI = 3.141592653589793;

mat3 getTBN(vec3 pos, vec2 tex, vec3 norm){
    vec3 u = dFdx(pos);
    vec3 v = dFdy(pos);
    vec2 t1 = dFdx(tex);
    vec2 t2 = dFdy(tex);

    // vec3 T = -normalize(u * t2.y - v * t1.y);
    // // vec3 T = -normalize(u - v);
    // vec3 N = normalize(norm);
    // vec3 B = normalize(cross(T, N));
    // return mat3(T, B, N);
    vec3 vp = cross(v, norm);
    vec3 up = cross(norm, u);
    vec3 T = vp * t1.x + up * t2.x;
    vec3 B = vp * t1.y + up * t2.y;

    float invmax = inversesqrt(max(dot(T, T), dot(B, B)));
    return mat3(T * invmax, B * invmax, norm);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0){
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness){
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness){
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness){
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}

void main(){
	float D = dot(FragPos, lightPos) * dot(FragPos, lightPos) - dot(lightPos, lightPos) * (dot(FragPos, FragPos) - 40960000);
	float t = (-dot(FragPos, lightPos) - sqrt(abs(D))) / dot(lightPos, lightPos);	
	vec3 Lo = vec3(0.0);

	vec3 albedo = vec3(texture(material.diffuse1, TexCoords));
	// float metallic = texture(material.surface1, TexCoords).z;
	float metallic = 0.2;
	float roughness = texture(material.surface1, TexCoords).x;
	float ao = texture(material.surface1, TexCoords).x;
	float em = texture(material.emission, TexCoords).x;
	// float em = 0;
	
	// mat3 TBN = getTBN(FragPos, TexCoords, Normal);

	if(D < 0 || t < 0){
		// float ao = 0.3;
		// float metallic = 0;
		// float roughness = 0.1;

		float radiance = 5;

		// vec3 N = vec3(0.0, 0.0, 1.0);
		vec3 N = TBN * normalize(vec3(texture(material.normal, TexCoords)) * 2.0 - vec3(1.0));
		// vec3 V = normalize(viewPos - FragPos);
		vec3 V = normalize(viewPos);
		vec3 L = normalize(normalize(lightPos));
		vec3 H = normalize(V + L);

		vec3 F0 = vec3(0.04);
		F0 = mix(F0, albedo, metallic);
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		vec3 specular = numerator / max(denominator, 0.001);

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;

		float NdotL = max(dot(N, L), 0.0);
		Lo = (kD * albedo / PI + specular) * radiance * NdotL;
		// Lo = albedo;
	}
	// }else{
	// 	Lo = vec3(0.0);
	// }
	// vec3 Lo = (kD * albedo / PI + specular - specular) * radiance * NdotL;
	// vec3 Lo = vec3(NdotL);

	// vec3 ambient = vec3(0.03) * albedo * ao;
	vec3 ambient = vec3(0.03) * albedo * ao + em;
	vec3 color = ambient + Lo;
	// vec3 color = Lo;

	FragColor = vec4(color, 1.0);
	// FragColor = vec4(vec3(0.5) + 0.5 * TBN * vec3(0.0, 0.0, 1.0), 1.0);
	// FragColor = vec4(0.5 * N + vec3(0.5), 1.0);
	// FragColor = vec4(N, 1);
}

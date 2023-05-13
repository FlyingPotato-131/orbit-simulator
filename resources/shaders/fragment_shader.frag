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
	sampler2D normalMap;
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
in vec3 lightDir;
in vec3 viewDir;
// in mat3 TBN;

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

    vec3 T = -normalize(u * t2.y - v * t1.y);
    vec3 N = normalize(norm);
    vec3 B = normalize(cross(T, N));
    return mat3(T, B, N);
}

// mat3 getTBN(vec3 p, vec2 uv, vec3 N){
// 	// get edge vectors of the pixel triangle
// 	vec3 dp1 = dFdx( p );
// 	vec3 dp2 = dFdy( p );
// 	vec2 duv1 = dFdx( uv );
// 	vec2 duv2 = dFdy( uv );
// 	// solve the linear system
// 	vec3 dp2perp = cross( dp2, N );
// 	vec3 dp1perp = cross( N, dp1 );
// 	vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
// 	vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
// 	// construct a scale-invariant frame
// 	float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
// 	return mat3( T * invmax, B * invmax, N );
// }

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
	mat3 TBN = getTBN(FragPos, TexCoords, Normal);

	vec3 albedo = vec3(texture(material.diffuse1, TexCoords));
	float metallic = texture(material.surface1, TexCoords).z;
	float roughness = texture(material.surface1, TexCoords).y;
	float ao = texture(material.surface1, TexCoords).x;
	// float ao = 0.3;
	// float metallic = 0;
	// float roughness = 0.1;

	float radiance = 5;

	vec3 N = TBN * normalize(vec3(texture(material.normalMap, TexCoords)) * 2.0 - vec3(1.0));
	// vec3 N = vec3(0.0, 0.0, 1.0);
	vec3 V = normalize(normalize(viewPos - FragPos + FragPos));
	vec3 L = normalize(normalize(light.position));
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
	vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;
	// vec3 Lo = (kD * albedo / PI + specular - specular) * radiance * NdotL;
	// vec3 Lo = vec3(NdotL);

	vec3 ambient = vec3(0.03) * albedo * ao;
	// vec3 ambient = vec3(1.03) * albedo * ao;
	vec3 color = ambient + Lo;
	// vec3 color = Lo;

	FragColor = vec4(color, 1.0);
	// FragColor = vec4(vec3(0.5) + 0.5 * TBN * vec3(0.0, 1.0, 0.0), 1.0);
	// FragColor = vec4(Normal, 1.0);
}

// void main(){
// 	// vec3 norm = vec3(0.0, 0.0, 1.0);
// 	vec3 norm = vec3(texture(material.normalMap, TexCoords));
// 	norm = normalize(norm * 2.0 - vec3(1.0));

// 	// float distance = length(light.position - FragPos);
// 	// float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
// 	float attenuation = 1.0;

// 	// vec3 ambient = lightColor * material.ambient * light.ambient;
// 	vec3 ambient = light.ambient * vec3(texture(material.diffuse1, TexCoords));
// 	// vec3 ambient = vec3(0.0);

// 	// diffuse
// 	// vec3 norm = normalize(Normal);
// 	// vec3 lightDir = normalize(light.position);
// 	// vec3 lightDir = normalize(light.position);
// 	float diff = max(dot(norm, lightPos), 0.0);
// 	// vec3 diffuse = light.diffuse * (diff);
// 	vec3 diffuse = diff * vec3(texture(material.surface1, TexCoords));
// 	// vec3 diffuse = light.diffuse * diff * vec3(texture(material.normalMap, TexCoords));
// 	// vec3 diffuse = light.diffuse * diff * vec3(transpose(TBN) * vec3(0.0, 1.0, 0.0) * inverse(TBN));
// 	// float gamma = 2.2;
// 	// vec3 diffuse = light.diffuse * diff * pow(texture(material.diffuse1, TexCoords).rgb, vec3(gamma));

// 	// specular
// 	// vec3 viewDir = TBN * normalize(viewDir - FragPos);
// 	// vec3 reflectdir = reflect(-lightDir, norm);
// 	// float spec = dot(reflectdir, norm) > 0 ? pow(max(dot(viewDir, reflectdir), 0.0), material.shininess) : 0;
// 	// // float spec = 1 > 0 ? pow(max(dot(viewdir, reflectdir), 0.0), material.shininess) : 0;
// 	// // vec3 specular = lightcolor * (spec * material.specular) * light.specular;
// 	// vec3 specular = light.specular * spec * vec3(texture(material.surface1, texcoords));
// 	// vec3 specular = vec3(0.0);

// 	vec3 lightDir = normalize(light.position - FragPos);
// 	vec3 viewDir = normalize(viewPos - FragPos);
// 	vec3 halfwayDir = normalize(lightDir + viewDir);

// 	float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
// 	vec3 specular = spec * vec3(texture(material.surface1, TexCoords));

// 	vec3 result = attenuation * (diffuse + specular) + ambient;
// 	FragColor = vec4(result, 1.0);	
// 	// FragColor = vec4(1.0);
// }
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

// out vec3 vertexColor;
out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
// out vec3 lightDir;
// out vec3 viewDir;
// out mat3 TBN;

struct Light{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

// uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalmtr;
uniform Light light;
uniform vec3 viewPos;

// uniform float xoffset;

void main(){
    // float far = 200;
    // float near = 0.01;

    // vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
    vec3 T = normalize(vec3(normalmtr * aTangent));
    // vec3 T = normalize(aTangent);
    vec3 N = normalize(vec3(normalmtr * aNormal));
    vec3 B = -cross(N, T);
    // TBN = transpose(mat3(T, B, N));
    // TBN = mat3(T, B, N);
    // TBN = mat3(vec3(0.0), vec3(0.0), N);

    gl_Position = projection * view * model * vec4(aPos, 1.0);
    // gl_Position = projection * view * vec4(aPos, 1.0);
    
    // gl_Position.z = 2.0*log(gl_Position.w/near + 1)/log(far/near + 1) - 1; 
    // gl_Position.z *= gl_Position.w;
    // gl_Position.z = 1 - exp(gl_Position.z);

    // Normal = vec3(model * vec4(aNormal, 1.0));
    // Normal = mat3(transpose(inverse(model))) * aNormal;
    Normal = normalmtr * aNormal;

    FragPos = vec3(model * vec4(aPos, 1.0));

    // lightDir = TBN * normalize(light.position);
    // viewDir = TBN * normalize(viewPos - FragPos);

    TexCoords = aTexCoords;
    // gl_PointSize = 10 * gl_Position.z;
}

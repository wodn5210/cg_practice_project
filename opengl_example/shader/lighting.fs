#version 330 core
in vec3 normal;
in vec2 texCoord;
in vec3 position;
out vec4 fragColor;
 
uniform vec3 viewPos;
 
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;
 
struct Material {
    vec3 ambient;     // 재질 자체가 빛나는 경우
    vec3 diffuse;
    vec3 specular;
    float shininess;  // 표면이 얼마나 반짝거리냐? -> 재질
};
uniform Material material;

void main() {
    vec3 ambient = material.ambient * light.ambient;
 
    vec3 lightDir = normalize(light.position - position);
    vec3 pixelNorm = normalize(normal);
    // normal을 normalize하는이유
    // normal vec 는 Vertex shader 에서 normalizing 이 되있ㄴ지만, rasterization 과정에서 vertex 사이의 있는 모든 fragment 에 대한 normal 을 보간하게됨
    // 그 과정에서 크기가 1이 아닐 수도 있음    
    float diff = max(dot(pixelNorm, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse * light.diffuse;
 
    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, pixelNorm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular * light.specular;
 
    vec3 result = ambient + diffuse + specular;


    fragColor = vec4(result, 1.0);
}
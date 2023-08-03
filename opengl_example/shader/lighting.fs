#version 330 core
in vec3 normal;
in vec2 texCoord;
in vec3 position;
out vec4 fragColor;
 
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float ambientStrength;
 
void main() {
    vec3 ambient = ambientStrength * lightColor;
    vec3 lightDir = normalize(lightPos - position);
    vec3 pixelNorm = normalize(normal);
    // normal을 normalize하는이유
    // normal vec 는 Vertex shader 에서 normalizing 이 되있ㄴ지만, rasterization 과정에서 vertex 사이의 있는 모든 fragment 에 대한 normal 을 보간하게됨
    // 그 과정에서 크기가 1이 아닐 수도 있음

    vec3 diffuse = max(dot(pixelNorm, lightDir), 0.0) * lightColor;
    vec3 result = (ambient + diffuse) * objectColor;
    fragColor = vec4(result, 1.0);
}
#version 330 core
in vec3 normal;
in vec2 texCoord;
in vec3 position;
out vec4 fragColor;
 
uniform vec3 viewPos;
 
struct Light {
    vec3 position;
	vec3 direction;
    vec2 cutoff;    // 어느 각도까지 비출것인가?
    vec3 attenuation;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;
 
struct Material {
	sampler2D diffuse;
    sampler2D specular;
    float shininess;  // 표면이 얼마나 반짝거리냐? -> 재질
};
uniform Material material;

void main() {
	vec3 texColor = texture(material.diffuse, texCoord).xyz;
    vec3 ambient = texColor * light.ambient;

	float dist = length(light.position - position);
    vec3 distPoly = vec3(1.0, dist, dist*dist);
    float attenuation = 1.0 / dot(distPoly, light.attenuation);
    vec3 lightDir = (light.position - position) / dist;


    float theta = dot(lightDir, normalize(-light.direction));
    float intensity = clamp((theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]), 0.0, 1.0);        
    vec3 result = ambient;


	if (intensity > 0.0) {
        vec3 pixelNorm = normalize(normal);
        float diff = max(dot(pixelNorm, lightDir), 0.0);
        vec3 diffuse = diff * texColor * light.diffuse;
        
        vec3 specColor = texture(material.specular, texCoord).xyz;
        vec3 viewDir = normalize(viewPos - position);
        vec3 reflectDir = reflect(-lightDir, pixelNorm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = spec * specColor * light.specular;
        
	    result += (diffuse + specular) * intensity;
    }

    result *= attenuation;

    fragColor = vec4(result, 1.0);

    // fs 는 x,y는 픽셀의 좌표, z는 0~1사이 normal 값
   // fragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}
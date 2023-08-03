#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 transform;         // project * view * model
uniform mat4 modelTransform;    // model 만 (local -> world)

out vec3 normal;
out vec2 texCoord;
out vec3 position;      // world 좌표만

void main() {
  gl_Position = transform * vec4(aPos, 1.0);  // 얘는 화면상의 -1 ~ +1 사이의 cononical space 으로 화면상 어디에 있는지 계산
  
  normal = (transpose(inverse(modelTransform)) * vec4(aNormal, 0.0)).xyz; // 얘도 diffuse 처럼 world 변환이 필요함
  

  texCoord = aTexCoord;
  position = (modelTransform * vec4(aPos, 1.0)).xyz;  // 얘는 world 위치만 (diffuse 는 world 좌표가 필요하다)
}
#version 330 core
in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

// 2D texture
uniform sampler2D tex;

void main() {
    // texture 함수를 통해서 바인딩된 tex 에 접근가능함
    fragColor = texture(tex, texCoord);
}
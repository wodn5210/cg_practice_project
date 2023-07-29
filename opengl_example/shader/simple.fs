#version 330 core
uniform vec4 color;
out vec4 fragColor;

/*
    uniform 사용할때 context.cpp 에서 program instance 생성이후 아래 과정 해주면된다.
    
	auto loc = glGetUniformLocation(m_program->Get(), "color");
    m_program->Use();
    glUniform4f(loc, 1.0f, 1.0f, 0.0f, 1.0f);
*/

void main() {
    fragColor = color;
}
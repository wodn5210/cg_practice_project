#include "program.h"

Program::~Program() {
    if (m_program) {
        glDeleteProgram(m_program);
    }
}


ProgramUPtr Program::Create(const std::vector<ShaderPtr>& shaders) {
    auto program = ProgramUPtr(new Program());
    if (!program->Link(shaders))
        return nullptr;

    return std::move(program);
}


bool Program::Link(const std::vector<ShaderPtr>& shaders) {
    m_program = glCreateProgram();
    for (auto& shader: shaders)                         // vector, set, map iterator는 for문 이렇게 변환 가능
        glAttachShader(m_program, shader->Get());       // shader id 를 program 에 attach 시킴
    glLinkProgram(m_program);

    int success = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(m_program, 1024, nullptr, infoLog);
        SPDLOG_ERROR("failed to link program: {}", infoLog);
        return false;
    }
    return true;
}

void Program::Use() const {
    glUseProgram(m_program);
}
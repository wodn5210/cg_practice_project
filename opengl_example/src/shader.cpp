#include "shader.h"


Shader::~Shader(){
    // m_shader 어떤 값이 있다는 뜻은 shader 가 아직 살아있다는 뜻
    if (m_shader){
        glDeleteShader(m_shader);
    }
}


ShaderUPtr Shader::CreateFromFile(const std::string& filename, GLenum shaderType) {
    // 내부함수여서 생성자 호출 가능
    auto shader = ShaderUPtr(new Shader());

    if (!shader->LoadFile(filename, shaderType))
        return nullptr;     // shader 메모리 해제될 것

    return std::move(shader);   //shader 권한 밖으로 넘김
}

bool Shader::LoadFile(const std::string& filename, GLenum shaderType) {
    auto result = LoadTextFile(filename);   // LoadTextFile은 optional 이기때문에 값이 있을수도, 없을수도 있다.
    if (!result.has_value())                // has_value 로 값이 있는지 확인
        return false;

    auto& code = result.value();
    // std::string& code 임
    // std::string code 로 사용한다면, result.value() 의 string 값을 그대로 복사해옴.
    // 하지만, result 변수는 LoadFile 스코프 내에 있기 때문에 소멸하지 않아서 복사할 필요 없음
    // auto 는 type 추측해서 std::string 으로 변환함. &여부는 판단을 못함
    // auto& 면 추론한 type의 reference type으로 가져옴

    const char* codePtr = code.c_str(); //c style 에서 사용하기위해 변환
    int32_t codeLength = (int32_t)code.length();

    // create and compile shader
    m_shader = glCreateShader(shaderType);      // shader를 하나 생성한다. -> 정수번호 하나 생성
    glShaderSource(m_shader, 1, (const GLchar* const*)&codePtr, &codeLength); 
    // 해당 shader에 얻은 소스 코드 설정
    // 여러 code 를 넣을 수 있도록 만들어서 복잡해 보이는것.
    // 여기서는 1개의 file 만 사용할거라 (id, 1, codeptr, codelength) 로 사용

    glCompileShader(m_shader); // 그 다음 compile

    // check compile error
    int success = 0;
    glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success);   
    // shader 의 정보를 얻어오는데, compile 상태를 가져온다.
    // iv -> int, vector(pointer) -> &success
    // compile 상태가 성공했는지 실패했는지 success 에 어떤 값이 써져있음.

    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(m_shader, 1024, nullptr, infoLog);
        SPDLOG_ERROR("failed to compile shader: \"{}\"", filename);
        SPDLOG_ERROR("reason: {}", infoLog);
        return false;
    }
    return true;
}
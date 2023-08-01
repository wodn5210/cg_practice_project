#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "common.h"
#include "shader.h"

CLASS_PTR(Program)
class Program {
public:
    // 여러 shader 를 받기 위해서 vector로 받고, 복사를 하지 않기위해 & 로 사용
    // shader ptr 은 여럿이 공동소유 가능
    // scope 를 벗어날때마다 ptr 을 소유하고 있는 개체수를 체크함
    // 소유수가 0이 되면 자동 delete 됨 => reference counting

    // unique ptr -> shared ptr 로 변환하는것은 쉬움
    // Shaderptr shader = Shader::CreateFromFile(); 자동 캐스팅 된다.
    // shared ptr -> unique ptr 로 변환은 어려움
    static ProgramUPtr Create(const std::vector<ShaderPtr>& shaders); 

    ~Program();
    uint32_t Get() const { return m_program; } 
    void Use() const;

    void SetUniform(const std::string &name, int value) const;
    void SetUniform(const std::string &name, float value) const;
    void SetUniform(const std::string &name, const glm::vec3 &value) const;
    void SetUniform(const std::string& name, const glm::mat4& value) const;

private:
    Program() {}
    bool Link(const std::vector<ShaderPtr>& shaders);
    uint32_t m_program { 0 };
};

#endif // __PROGRAM_H__
#ifndef __SHADER_H__
#define __SHADER_H__

#include "common.h"

CLASS_PTR(Shader);
// class Shader; 
// using ShaderUPtr = std::unique_ptr<Shader>; 
// using ShaderPtr = std::shared_ptr<Shader>; 
// using ShaderWPtr = std::weak_ptr<Shader>;


class Shader {
public:
    //class 를 생성할 유일한 방법은 CreateFromFile 함수를 호출해야함.
    static ShaderUPtr CreateFromFile(const std::string& filename, GLenum shaderType);

    ~Shader();
    uint32_t Get() const { return m_shader; }    

private:
    Shader() {} // constructor 가 private
    bool LoadFile(const std::string& filename, GLenum shaderType);    
    
    // opengl 에서 얻어진 shader id
    uint32_t m_shader { 0 };        
};

#endif // __SHADER_H__
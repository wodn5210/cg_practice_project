#include "common.h"
#include <fstream>
#include <sstream>

std::optional<std::string> LoadTextFile(const std::string& filename) {
    //c++ style 의 파일 읽기
    std::ifstream fin(filename);                                
    if (!fin.is_open()) {
        SPDLOG_ERROR("failed to open file: {}", filename);
        return {};  // optional type : 아무것도 없는경우
    }

    std::stringstream text;
    text << fin.rdbuf();
    return text.str();
}

glm::vec3 GetAttenuationCoeff(float distance){
    const auto linear_coeff = glm::vec4(
        8.4523112e-05, 4.4712582e+00, -1.8516388e+00, 3.3955811e+01);
    const auto quad_coeff = glm::vec4(
        -7.6103583e-04, 9.0120201e+00, -1.1618500e+01, 1.0000464e+02);

    float kc = 1.0f;
    float d = 1.0f / distance;
    auto dvec = glm::vec4(1.0f, d, d * d, d * d * d);
    float kl = glm::dot(linear_coeff, dvec);
    float kq = glm::dot(quad_coeff, dvec);

    return glm::vec3(kc, glm::max(kl, 0.0f), glm::max(kq * kq, 0.0f));
}

// optional<> 어떤값이 있거나 없는 경우를 포인터 없이 표현 가능

// 만약 파일없으면 비어있는 string 을 읽는다.?
// 보통은 std::string* Load... 이런식으로 쓰는데 이러면 NULL 로 받을 수 있음
// 근데 이런거는 동적할당이 들어가게 되고, 관리 까먹으면 메모리 문제 생길 수 있음

// optional<string> 이런식으로 하면 값이 있는경우에는 string 사용되고
// 값이 없다면 못꺼내고,..


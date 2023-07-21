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

// optional<> 어떤값이 있거나 없는 경우를 포인터 없이 표현 가능

// 만약 파일없으면 비어있는 string 을 읽는다.?
// 보통은 std::string* Load... 이런식으로 쓰는데 이러면 NULL 로 받을 수 있음
// 근데 이런거는 동적할당이 들어가게 되고, 관리 까먹으면 메모리 문제 생길 수 있음

// optional<string> 이런식으로 하면 값이 있는경우에는 string 사용되고
// 값이 없다면 못꺼내고,..


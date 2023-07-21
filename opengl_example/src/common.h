#ifndef __COMMON_H__    
#define __COMMON_H__
// == 윈도우의 경우 #pragma once 도 사용가능

// common.h include 할때
// __COMMON_H__ 가 define 되지 않았다면, 아래것들 넣는다.

//만약 이 헤더가 없으면, 실수로 여러번 include 했을때, 아래 내용이 3번 들어간다.
// 함수가 두번 선언되어 에러남

#include <memory>
#include <string>
#include <optional>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <spdlog/spdlog.h>


#define CLASS_PTR(klassName) \
class klassName; \
using klassName ## UPtr = std::unique_ptr<klassName>; \
using klassName ## Ptr = std::shared_ptr<klassName>; \
using klassName ## WPtr = std::weak_ptr<klassName>;



std::optional<std::string> LoadTextFile(const std::string& filename);


#endif // __COMMON_H__
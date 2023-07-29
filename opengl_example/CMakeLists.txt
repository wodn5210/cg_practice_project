cmake_minimum_required(VERSION 3.13)

set(PROJECT_NAME texture_example)
set(CMAKE_CXX_STANDARD 17)

set(WINDOW_NAME "Texture Example")
set(WINDOW_WIDTH 960)
set(WINDOW_HEIGHT 540)


project(${PROJECT_NAME})    # 본격적으로 프로젝트 세팅
#Project name 이라는 실행파일 생성할것인데, 뒤의 main.cpp 를 이용함
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/common.cpp src/common.h
    src/shader.cpp src/shader.h
    src/program.cpp src/program.h   
    src/context.cpp src/context.h   
    src/buffer.cpp src/buffer.h     
    src/vertex_layout.cpp src/vertex_layout.h   
    src/image.cpp src/image.h            
    src/texture.cpp src/texture.h              
    )

include(dependency.cmake)

# 우리 프로젝트에 include / lib 관련 옵션 추가
target_include_directories(${PROJECT_NAME} PUBLIC ${DEP_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${DEP_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${DEP_LIBS})

# macos 추가
if(APPLE)
    target_link_libraries(${PROJECT_NAME} PUBLIC "-framework OpenGL")
    target_link_libraries(${PROJECT_NAME} PUBLIC "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} PUBLIC "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} PUBLIC "-framework CoreVideo")
    target_link_libraries(${PROJECT_NAME} PUBLIC "-framework CoreFoundation")
else()

endif()

# c코드에서 Define 으로 사용할 수 있도록 만들어준다
target_compile_definitions(${PROJECT_NAME} PUBLIC
    WINDOW_NAME="${WINDOW_NAME}"
    WINDOW_WIDTH=${WINDOW_WIDTH}
    WINDOW_HEIGHT=${WINDOW_HEIGHT}
    )

# Dependency들이 먼저 build 될 수 있게 관계 설정
add_dependencies(${PROJECT_NAME} ${DEP_LIST})   #List 안에 있는것들 먼저 컴파일 한 뒤, 우리 프로젝트 컴파일 가능



# cmake -B빌드폴더 .
# cmake --build 빌드폴더

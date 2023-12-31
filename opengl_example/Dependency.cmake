#ExternalProject 관련 명령어 셋 추가 - include: cmake 에서 추가기능할때 -> ExternalProject_Add 관련
include(ExternalProject)

# ${PROJECT_BINARY_DIR}/: build폴더
# Dependency 관련 변수 설정
set(DEP_INSTALL_DIR ${PROJECT_BINARY_DIR}/install)
set(DEP_INCLUDE_DIR ${DEP_INSTALL_DIR}/include)
set(DEP_LIB_DIR ${DEP_INSTALL_DIR}/lib)

# spdlog: fast logger library
ExternalProject_Add(                                                # git 에서 다운받은다음 cmake를 실행해준다.
    dep_spdlog                                                  #프로젝트의 대표이름
    GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
    GIT_TAG "v1.x"                                              #git 페이지에서 branch 버전이름이나 태그를 설정해주면된다.
    GIT_SHALLOW 1                                               #shallow 활성화(가장최신의 버전만 다운받는다.)
    UPDATE_COMMAND ""                                           # 이하내용은 clone 해서 다운받았을때 빌드를 어떤식으로 할지 (최신버전 업데이트)
    PATCH_COMMAND ""                                            # 받은내용 뭔가 수정할때
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}        # cmake 의 config할때 인자를 어떻게 줄것인가?   (-D변수=값)
    TEST_COMMAND ""                                             
)

# Dependency 리스트 및 라이브러리 파일 리스트 추가
set(DEP_LIST ${DEP_LIST} dep_spdlog)
if(APPLE)
    set(DEP_LIBS ${DEP_LIBS} spdlog)
else()
    set(DEP_LIBS ${DEP_LIBS} spdlog$<$<CONFIG:Debug>:d>)           # mac 에선 안됨
endif()

# # glfw
ExternalProject_Add(
    dep_glfw
    GIT_REPOSITORY "https://github.com/glfw/glfw.git"
    GIT_TAG "3.3.4"
    GIT_SHALLOW 1
    UPDATE_COMMAND "" 
    PATCH_COMMAND "" 
    TEST_COMMAND ""
    CMAKE_ARGS                                          # openglfw 에서 사용할 옵션들 - 해당 makefile에 보면 있음
        -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}       # speed log와 옵션 동일
        -DGLFW_BUILD_EXAMPLES=OFF                       # 예제, 테트트파일, 문서
        -DGLFW_BUILD_TESTS=OFF
        -DGLFW_BUILD_DOCS=OFF
    )
set(DEP_LIST ${DEP_LIST} dep_glfw)
set(DEP_LIBS ${DEP_LIBS} glfw3)

# glad
ExternalProject_Add(
    dep_glad
    GIT_REPOSITORY "https://github.com/Dav1dde/glad"
    GIT_TAG "v0.1.34"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
        -DGLAD_INSTALL=ON
    TEST_COMMAND ""
    )
set(DEP_LIST ${DEP_LIST} dep_glad)
set(DEP_LIBS ${DEP_LIBS} glad)


# stb
# single file library 이기때문에 lib 의존성 추가할 필요는 없음 (빌드 필요 x)
# single file -> .h 파일 하나로만 사용 가능
ExternalProject_Add(
    dep_stb
    GIT_REPOSITORY "https://github.com/nothings/stb"
    GIT_TAG "master"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CONFIGURE_COMMAND ""            # build 필요  X
    BUILD_COMMAND ""                # build 필요  X
    TEST_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy                            # ${CMAKE_COMMAND} == cmake 임. cmake 의 copy 명령어
        ${PROJECT_BINARY_DIR}/dep_stb-prefix/src/dep_stb/stb_image.h    # 다운받아진 file 중 stb_image.h 만
        ${DEP_INSTALL_DIR}/include/stb/stb_image.h                      # 우리 project 폴더에 복사한다
    )
set(DEP_LIST ${DEP_LIST} dep_stb)

# glm header only 라이브러리 - stb library 와 비슷
ExternalProject_Add(
    dep_glm
    GIT_REPOSITORY "https://github.com/g-truc/glm"
    GIT_TAG "0.9.9.8"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    TEST_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_BINARY_DIR}/dep_glm-prefix/src/dep_glm/glm
        ${DEP_INSTALL_DIR}/include/glm
    )
set(DEP_LIST ${DEP_LIST} dep_glm)


# imgui
# 외부 프로젝트가아닌, 내부 프로젝트 형테로 빌드해서 Debug 폴더내에 lib 파일이 생긴다.
add_library(imgui                    #target name, 코드 따로수정할 예정없기때문에 cpp 파일만 추가했다
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_opengl3.cpp
    )
target_include_directories(imgui PRIVATE ${DEP_INCLUDE_DIR})        
add_dependencies(imgui ${DEP_LIST})         # dep list 들 먼저 빌드된뒤에 imgui 빌드하자
set(DEP_INCLUDE_DIR ${DEP_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(DEP_LIST ${DEP_LIST} imgui)
set(DEP_LIBS ${DEP_LIBS} imgui)

# assimp
ExternalProject_Add(
    dep_assimp
    GIT_REPOSITORY "https://github.com/assimp/assimp"
    GIT_TAG "v5.0.1"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
        -DBUILD_SHARED_LIBS=OFF
        -DASSIMP_BUILD_ASSIMP_TOOLS=OFF
        -DASSIMP_BUILD_TESTS=OFF
        -DASSIMP_INJECT_DEBUG_POSTFIX=OFF
        -DASSIMP_BUILD_ZLIB=ON
    TEST_COMMAND ""
    )
set(DEP_LIST ${DEP_LIST} dep_assimp)
# Macos 에서는 아래 파일명이 다를 수 있어서 수정필요함
# ex) assimp이렇게 변경?
#   
if(APPLE)
    set(DEP_LIBS ${DEP_LIBS}
        assimp
        zlibstatic
        IrrXML
        )
else()
    set(DEP_LIBS ${DEP_LIBS}
        assimp-vc142-mt$<$<CONFIG:Debug>:d>
        zlibstatic$<$<CONFIG:Debug>:d>
        IrrXML$<$<CONFIG:Debug>:d>
        )
endif()
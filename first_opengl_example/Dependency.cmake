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
set(DEP_LIBS ${DEP_LIBS} spdlog)
#set(DEP_LIBS ${DEP_LIBS} spdlog$<$<CONFIG:Debug>:d>)           # 이거 안됨 ㅜ


# # glfw
ExternalProject_Add(
    dep_glfw
    GIT_REPOSITORY "https://github.com/glfw/glfw.git"
    GIT_TAG "3.3.8"
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

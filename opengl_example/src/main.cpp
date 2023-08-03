#include "context.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

/*
    Context 의 전역변수는 선언하지 말자.
    초기화와 지우는 위치를 정확히 알 수 없기때문에
    User Pointer 를 사용할것
    glfwSetWindowUserPointer(window, context.get());
*/
void OnCursorPos(GLFWwindow* window, double x, double y) {
  auto context = (Context*)glfwGetWindowUserPointer(window);
  context->MouseMove(x, y);
}

void OnMouseButton(GLFWwindow* window, int button, int action, int modifier) {
  ImGui_ImplGlfw_MouseButtonCallback(window, button, action, modifier);
  
  auto context = (Context *)glfwGetWindowUserPointer(window);
  double x, y;
  glfwGetCursorPos(window, &x, &y);
  context->MouseButton(button, action, x, y);
  SPDLOG_INFO("OnMouse");
}

void OnCharEvent(GLFWwindow* window, unsigned int ch) {
    ImGui_ImplGlfw_CharCallback(window, ch);
}

void OnScroll(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

// 윈도우 크기가 변경될 때마다 호출
void OnFramebufferSizeChange(GLFWwindow* window, int width, int height) {
    glfwGetFramebufferSize(window, &width, &height);    // 맥에서는 이상하게 화면크기조절시 framebuffer size 도 지정해야한다.
    // 추가설명으로는 macOs 에서는 스크린 좌표와 픽셀이 1:1 매치가 아니여서 그렇단다

    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);
    //auto context = (Context *)glfwGetWindowUserPointer(window); // C-type Casting
    auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));  // void -> Context* 로 아예 변경? 하는거라 reinterpret 사용해야한대
    context->Reshape(width, height);    // 그림그릴 위치 및 화면크기 설정
}

void OnKeyEvent(GLFWwindow* window,
    int key, int scancode, int action, int mods) {

    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);   

    SPDLOG_INFO("key: {}, scancode: {}, action: {}, mods: {}{}{}",
        key, scancode,
        action == GLFW_PRESS ? "Pressed" :
        action == GLFW_RELEASE ? "Released" :
        action == GLFW_REPEAT ? "Repeat" : "Unknown",
        mods & GLFW_MOD_CONTROL ? "C" : "-",
        mods & GLFW_MOD_SHIFT ? "S" : "-",
        mods & GLFW_MOD_ALT ? "A" : "-");
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}


int main()
{    
    SPDLOG_INFO("Start Program");
    SPDLOG_INFO("Initialize glfw");
    if (!glfwInit()) {
        const char* description = nullptr;
        glfwGetError(&description);
        SPDLOG_ERROR("failed to initialize glfw: {}", description);
        return -1;
    }

#ifdef APPLE
    SPDLOG_INFO("APPLE");
#endif

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);              //macos 에서는 3.3으로 해도 4.1로 변경됨
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);              //내 window os컴퓨터에서는 4.1 로 설정시 window 생성 fail 발생
    
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);        //macos 에러 추가
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw 윈도우 생성, 실패하면 에러 출력후 종료
    SPDLOG_INFO("Create glfw window");
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME,
      nullptr, nullptr);
    if (!window) {
        SPDLOG_ERROR("failed to create glfw window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);         // 지금 대상이되는 window의 context에 대해서 실행해라.

    // glad를 활용한 OpenGL 함수 로딩 (context 생성이후 - window 생성할때 생김)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    SPDLOG_ERROR("failed to initialize glad");
       glfwTerminate();
       return -1;
    }
    auto glVersion = glGetString(GL_VERSION);
    SPDLOG_INFO("OpenGL context version: {}", (const char*)glVersion);  // (const char*) -> unsigned char* 형에 대한 {} 로그 지원이 없어진듯


    // IMgui 는 glContext 생성 이후에
    auto imguiContext = ImGui::CreateContext();         // IMGUI 에서 제공하는 Context 생성
    ImGui::SetCurrentContext(imguiContext);             // 해당 Context 를 현재 Context 로 설정
    ImGui_ImplGlfw_InitForOpenGL(window, false);        // 두번째인자는 callback 사용할것이냐? 안함 -> 우린 직접한다
    
    // opengl3 버전에 대한 Init 을 위해 사용. shader, font, texture 등등
    ImGui_ImplOpenGL3_Init();                           
    ImGui_ImplOpenGL3_CreateFontsTexture();
    ImGui_ImplOpenGL3_CreateDeviceObjects();

    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
    glEnable(GL_DEPTH_TEST);

    // 여기까지 초기화 끝.
    // 이 이후부터 opengl function 본격적으로 사용 가능
    auto context = Context::Create();
    if (!context) {
        SPDLOG_ERROR("failed to create context");
        glfwTerminate();
        return -1;
    }

    // Context 전역 대신 사용
    glfwSetWindowUserPointer(window, context.get());
    // auto context = (Context*)glfwGetWindowUserPointer(window); 이렇게 사용

    
    OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);       // 첫 윈도우 생성직후에는 이벤트 발생 안하므로 강제로 호출 (위치는 중요 X)
    
    // 이벤트 함수 등록
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
    glfwSetKeyCallback(window, OnKeyEvent);
    glfwSetCharCallback(window, OnCharEvent);
    glfwSetCursorPosCallback(window, OnCursorPos);
	glfwSetMouseButtonCallback(window, OnMouseButton);
    glfwSetScrollCallback(window, OnScroll);


    // glfw 루프 실행, 윈도우 close 버튼을 누르면 정상 종료
    SPDLOG_INFO("Start main loop");
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   

        glfwPollEvents();   // 이벤트 수집 ex) 창움직이기, 마우스, 키보드 ...
        // 화면크기 변경 감지된거 확인하고 OnFramebufferSizeChange함수 불림

        // Imgui는 매 프레임마다 gui 그리기때문에 새 Frame 인걸 알려준다.
	    ImGui_ImplGlfw_NewFrame();  // display size setting
        ImGui::NewFrame();

        // Poll 전에하든 후에하는 상관없음
        context->ProcessInput(window);
        context->Render();

        // 우리가 그린 Scene 위에 gui 를 그릴것임.
	    ImGui::Render();        // imgui ui 코드들 쫙 조합해서 그림그려야하는 정보들 list 형태로 생김
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // 여기서 그 list들을 실제로 그린다.

        // 화면에 그림그릴때 버퍼 2개를 준비
        // 하나로만하면 그림그려지는 과정도 보이므로
        // 그림 다 완성된거 보여주고, 나머지에 그림
        glfwSwapBuffers(window); //=> double buffering

    }

    // context 의 메모리 모두 해제
    // context = nullptr;
    context.reset();

    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_DestroyDeviceObjects();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(imguiContext);

    glfwTerminate();
    return 0;
}
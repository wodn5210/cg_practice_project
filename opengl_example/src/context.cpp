#include "context.h"
#include "image.h"
#include <imgui.h>

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;

    return std::move(context);
}

bool Context::Init() { 
    m_box = Mesh::CreateBox();    

    m_simpleProgram = Program::Create("./shader/simple.vs", "./shader/simple.fs");
    if (!m_simpleProgram)
        return false;

    m_program = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");
    if (!m_program)
        return false;

    auto image = Image::Load("./image/container.jpg");      // 이미지들은 항상 메모리에 들고있을 필요없다. texture 만들면 그만임.
    if (!image) 
        return false;
    SPDLOG_INFO("image: {}x{}, {} channels",
        image->GetWidth(), image->GetHeight(), image->GetChannelCount());
	m_texture = Texture::CreateFromImage(image.get());

	auto image2 = Image::Load("./image/awesomeface.png");
    if (!image2) 
        return false;
    SPDLOG_INFO("image2: {}x{}, {} channels",
        image2->GetWidth(), image2->GetHeight(), image2->GetChannelCount());    
    m_texture2 = Texture::CreateFromImage(image2.get());

    // m_material.diffuse = Texture::CreateFromImage(Image::Load("./image/container2.png").get());
	// m_material.specular = Texture::CreateFromImage(Image::Load("./image/container2_specular.png").get());
    m_material.diffuse = Texture::CreateFromImage(Image::CreateSingleColorImage(4, 4, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)).get());
    m_material.specular = Texture::CreateFromImage(Image::CreateSingleColorImage(4, 4, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());


    glActiveTexture(GL_TEXTURE0);                       // 사용할 텍스쳐 슬롯 번호 등록
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());     // 0번 슬롯에는 2D Texture Id m_texture 이다
    glActiveTexture(GL_TEXTURE1);                       // 사용할 텍스쳐 슬롯 번호 1번으로 변경
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());    // 1번 슬롯에는 2D Texture m_texture2 이다

    m_program->Use();
    m_program->SetUniform("tex", 0);
    m_program->SetUniform("tex2", 1);


    glBindVertexArray(0);
    glUseProgram(0);
    return true;
}

void Context::Render() {
    // Begin ~ End 는 항상 쌍으로 되어야한다. 이게 윈도우 띄우는 기본

    if (ImGui::Begin("ui window"))  // 윈도우가 열려있으면
    {
        if (ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor))) //m_clearColor 색상 변경
        {
            // 그리고 실제 값의 변경이 있을때만 if 문 안으로 진입
            // Clear Color 색 변경
            glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
        }
        ImGui::Separator();
        ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);        // 맨끝에는 speed
        ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f);                         // 맨끝에는 speed
        ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);      // 맨끝에는 speed, 값 제한
        ImGui::Separator();
        if (ImGui::Button("reset camera"))
        {
            // 버튼 눌리면 진입
            m_cameraYaw = 0.0f;
            m_cameraPitch = 0.0f;
            m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        }
        if (ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat3("l.position", glm::value_ptr(m_light.position), 0.01f);
            ImGui::DragFloat3("l.direction", glm::value_ptr(m_light.direction), 0.01f);
            ImGui::DragFloat2("l.cutoff", glm::value_ptr(m_light.cutoff), 0.5f, 0.0f, 180.0f);
            ImGui::DragFloat("l.distance", &m_light.distance, 0.5f, 0.0f, 3000.0f);
            ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient));
            ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse));
            ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular));
            ImGui::Checkbox("flash light", &m_flashLightMode);
        }

        if (ImGui::CollapsingHeader("material", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat("m.shininess", &m_material.shininess, 1.0f, 1.0f, 256.0f);
        }
        ImGui::Checkbox("animation", &m_animation);
    }
    ImGui::End();


    /*
     *  Open GL 은 열 우선방식으로, 이론상 나와있는 배열의 형태와 다르게 나타날 수 있다.
     * 가령 행렬 A:translate 예를 보자
     * 강의에서는 A03, A13, A23 의 부분이 translate 부분이다.
     * 하지만 glm::translate 에서는 A30,A31,A32 가 translate 가 된다.
     */
    m_cameraFront =
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    auto projection = glm::perspective(glm::radians(45.0f), (float)m_width / (float)m_height, 0.01f, 20.0f);
    auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);



    glm::vec3 lightPos = m_light.position;
    glm::vec3 lightDir = m_light.direction;
    if(m_flashLightMode){
        lightPos = m_cameraPos;
        lightDir = m_cameraFront;
    }
    else{
        // after computing projection and view matrix
        auto lightModelTransform = glm::translate(glm::mat4(1.0), m_light.position) * glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
        m_simpleProgram->Use();
        m_simpleProgram->SetUniform("color", glm::vec4(m_light.ambient + m_light.diffuse, 1.0f));
        m_simpleProgram->SetUniform("transform", projection * view * lightModelTransform);
        m_box->Draw(m_simpleProgram.get());
    }

    m_program->Use();
    m_program->SetUniform("viewPos", m_cameraPos);
    m_program->SetUniform("light.position", lightPos);
    m_program->SetUniform("light.direction", lightDir);
    m_program->SetUniform("light.cutoff", glm::vec2(    cosf(glm::radians(m_light.cutoff[0])),
                                                        cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
    m_program->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
    m_program->SetUniform("light.ambient", m_light.ambient);
    m_program->SetUniform("light.diffuse", m_light.diffuse);
    m_program->SetUniform("light.specular", m_light.specular);

    m_program->SetUniform("material.diffuse", 0);  
    m_program->SetUniform("material.specular", 1);        
    m_program->SetUniform("material.shininess", m_material.shininess);
  



    glBindVertexArray(0);
    glUseProgram(0);
}

void Context::ProcessInput(GLFWwindow* window) {
    if (!m_cameraControl)
        return;
    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * m_cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * m_cameraFront;

    auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraRight;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraRight;

    auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraUp;
}

void Context::Reshape(int width, int height) {
    m_width = width;
    m_height = height;

    glViewport(0, 0, m_width, m_height);
}

void Context::MouseMove(double x, double y)
{
    if (!m_cameraControl)
        return;

    static glm::vec2 prevPos = glm::vec2((float)x, (float)y);
    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - prevPos;

    const float cameraRotSpeed = 0.8f;
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    if (m_cameraYaw < 0.0f)
        m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f)
        m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f)
        m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f)
        m_cameraPitch = -89.0f;

    prevPos = pos;
}

void Context::MouseButton(int button, int action, double x, double y)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            // 마우스 조작 시작 시점에 현재 마우스 커서 위치 저장
            m_prevMousePos = glm::vec2((float)x, (float)y);
            m_cameraControl = true;
        }
        else if (action == GLFW_RELEASE)
        {
            m_cameraControl = false;
        }
    }
}
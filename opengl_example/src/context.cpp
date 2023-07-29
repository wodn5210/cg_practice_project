#include "context.h"
#include "image.h"

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;

    return std::move(context);
}

bool Context::Init() {
    float vertices[] = {
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    };
    uint32_t indices[] = { // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };

/*
    Vertex Array Object 생성 순서
    VAO binding -> VBO binding -> vertex attribute setting
    -> EBO 생성
*/
/*
    //VBO 생성 -> buffer object 만들기 전에 만들어야한다.
    // vertex array object = 정점 데이터의 구조를 알려주는 object
    // 각 정점은 몇byte? 정점은 몇 byte 만큼 떨어져있나? 
    // => 일반화해서 attribute 구조에 대한 얘기    
    glGenVertexArrays(1, &m_vertexArrayObject);
    glBindVertexArray(m_vertexArrayObject);
*/    

    m_vertexLayout = VertexLayout::Create();

    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 32);

    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);                      // vertex
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 3);      // color
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 6);      // texcod

	m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 6);


    ShaderPtr vertShader = Shader::CreateFromFile("./shader/texture.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/texture.fs", GL_FRAGMENT_SHADER);
    if (!vertShader || !fragShader)
        return false;
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

    m_program = Program::Create({fragShader, vertShader});  //vector 형태로 들어감
    if (!m_program)
        return false;    SPDLOG_INFO("program id: {}", m_program->Get());

    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

    auto image = Image::Load("./image/container.jpg"); 
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

    glActiveTexture(GL_TEXTURE0);                       // 사용할 텍스쳐 슬롯 번호 등록
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());     // 0번 슬롯에는 2D Texture Id m_texture 이다
    glActiveTexture(GL_TEXTURE1);                       // 사용할 텍스쳐 슬롯 번호 1번으로 변경
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());    // 1번 슬롯에는 2D Texture m_texture2 이다

    m_program->Use();
    glUniform1i(glGetUniformLocation(m_program->Get(), "tex"), 0);      // 0번 텍스쳐 슬롯
    glUniform1i(glGetUniformLocation(m_program->Get(), "tex2"), 1);     // 1번 텍스쳐 슬롯 

/*
    이미지 상하반전 되어있는 이유?

    이미지는 좌상단이 0,0 인데, Opengl은 좌하단을 0,0 으로 생각함
    이미지 로딩할때 상하반전 시켜보자
*/    

    // 0.5배 축소후 z축으로 90도 회전하는 행렬
    auto transform = glm::rotate(
        glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)),
        glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)
        );
    auto transformLoc = glGetUniformLocation(m_program->Get(), "transform");        // uniform location 얻어오기
    // location, 몇개?(배열), transpose 여부, pointer
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));       // 값 넣기

    return true;
}

void Context::Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    m_program->Use();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


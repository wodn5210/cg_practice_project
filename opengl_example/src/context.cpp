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

    glGenTextures(1, &m_texture);   // 한장 만들겠다.
    glBindTexture(GL_TEXTURE_2D, m_texture);    // 2D Texture 바인딩 한다.
    // 텍스쳐 속성 i = 지정하려는 값이 정수
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);       // 축소됐을때 필터
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);       // 확대됐을때 필터
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);    // 텍스쳐 벗어난경우 (S: 가로)  CLAME to Edge: 끝값으로 설정
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);    // 텍스쳐 벗어난경우 (T: 세로)

    // (타겟(위에서 바인딩한놈), Level(minmap 0:기본 이미지 사이즈 - 커질수록 이미지사이즈 작아짐) )
    glTexImage2D(   GL_TEXTURE_2D, 0, 
                                                                        // 아래는 gpu 에서 사용하는 속성. GL_R만하면 붉은색만 출력
                                                                        // GL_RGBA8 로해도 아래 실제 이미지가 RGB라서 똑같이나옴
                    GL_RGB, image->GetWidth(), image->GetHeight(), 0,   // GPU 쪽에서 사용할 채널타입 RGB, 이미지 사이즈W, 이미지H, border size(테두리) 
                                                                        // 아래 부분은 이미지의 속성에 대한부분임.
                    GL_RGB, GL_UNSIGNED_BYTE, image->GetData());        // 실제 이미지에서의 type, 이미지가 하나의 channel을 표현할때 사용하는 단위 byte, 실제 이미지의 포인터

    return true;
}

void Context::Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    m_program->Use();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


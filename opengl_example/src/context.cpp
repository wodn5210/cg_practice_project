#include "context.h"

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;

    return std::move(context);
}

bool Context::Init() {
    float vertices[] = {
        0.5f, 0.5f, 0.0f, // top right
        0.5f, -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, // top left
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
    // glGenVertexArrays + glBindVertexArray
    m_vertexLayout = VertexLayout::Create();

/*
    // vertex buffer 생성 (위 bind 한 VBO 에 자동으로 설정된다)
    glGenBuffers(1, &m_vertexBuffer);           
    // vertex(array) 버퍼 or element(index) 버퍼 선택
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    
    // buffer 에 데이터를 실제 복사.
    // STATIC/DINAMIC/STREAM - DRAW/READ/COPY 조합
    // static draw 는 vertex 위치를 변경 안할것이다.
    // stream 은 버퍼 한번 세팅하고 몇번쓰다가 버린다.
    // dynamic 은 데이터가 자주 변경된다
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, vertices, GL_STATIC_DRAW);   
*/
    // glGenBuffers + glBindBuffer + glBufferData
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 12);

/*
	glEnableVertexAttribArray(0);   // 0번 attribute 활성화
    // 0번attribute인데 /데이터3개씩에/float형이고 /노멀라이즈 필요없고/ 다음 점까지는 3개 건너뛰고/ offset은 0이다
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
*/
    // glEnableVertexAttribArray + glVertexAttribPointer
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);


/*
    // EBO: index buffer 라고 생각하면 됨
    // attribute 따로 설정할필요없음. 정수값만 들어오니까(index)
	glGenBuffers(1, &m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 6, indices, GL_STATIC_DRAW);    
*/
    // glGenBuffers + glBindBuffer + glBufferData
	m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 6);


    ShaderPtr vertShader = Shader::CreateFromFile("./shader/simple.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/simple.fs", GL_FRAGMENT_SHADER);
    if (!vertShader || !fragShader)
        return false;
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

    m_program = Program::Create({fragShader, vertShader});  //vector 형태로 들어감
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
    return true;
}

void Context::Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    m_program->Use();

    // 어떤타입, 몇번째 점부터 그릴지, 몇개의 점을 그릴지
    // 지금 VAO binding 되어있는 데이터 기준임
    // glDrawArrays(GL_TRIANGLES, 0, 6);

    // EBO 를 사용할경우 아래 함수로 사용해야한다.
    // index 의 수, 어떤 type의 index?
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}


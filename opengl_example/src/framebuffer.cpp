#include "framebuffer.h"

FramebufferUPtr Framebuffer::Create(const TexturePtr colorAttachment)
{
    auto framebuffer = FramebufferUPtr(new Framebuffer());
    if (!framebuffer->InitWithColorAttachment(colorAttachment))
        return nullptr;
    return std::move(framebuffer);
}

Framebuffer::~Framebuffer()
{
    if (m_depthStencilBuffer)
    {
        glDeleteRenderbuffers(1, &m_depthStencilBuffer);
    }
    if (m_framebuffer)
    {
        glDeleteFramebuffers(1, &m_framebuffer);
    }
}

void Framebuffer::BindToDefault()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);   //default framebuffer binding : 화면에 그린다.
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

bool Framebuffer::InitWithColorAttachment(const TexturePtr colorAttachment)
{
    m_colorAttachment = colorAttachment;
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);       //여기서도 0을넣으면 default framebuffer - 지금사용중인 buffer

    // Frame Buffer 에 Texture 붙임. (COlor Buffer임) 
    // 내가 어떤 Color, deipth, stencil 등에 texture를 사용하고 싶을때 사용
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           colorAttachment->Get(), 0);

    // 위에 Color Buffer 만들었으니, Depth, Stencil Buffer 만들자
    // texture 로 만들진 않고 바로 Render Buffer 로 만들것
    glGenRenderbuffers(1, &m_depthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);

    // 이 렌더버퍼가 어떤타입인가 지정.
    glRenderbufferStorage(  GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                            colorAttachment->GetWidth(), colorAttachment->GetHeight());
    glBindRenderbuffer(GL_RENDERBUFFER, 0);     // 0이면 default renderbuffer로 바인딩하겠다.
    // 0을 넣으면 default binding 된다. ex) program binding 시에 0넣으면 지금 사용중인 program


    // binding 했던 frame버퍼에 depth stencil 을 붙인다.
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER, m_depthStencilBuffer);

    // Frame buffer 가 사용가능한지
    auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE)
    {
        // Fail했으면 Frame buffer 조합이 실패했다는 뜻. hw 지원을 안한다거나, size가 안맞다거나
        SPDLOG_ERROR("failed to create framebuffer: {}", result);
        return false;
    }

    BindToDefault();

    return true;
}
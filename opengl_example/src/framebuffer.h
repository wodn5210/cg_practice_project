#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "texture.h"

CLASS_PTR(Framebuffer);
class Framebuffer {
public:
    static FramebufferUPtr Create(const TexturePtr colorAttachment);
    static void BindToDefault();
    ~Framebuffer();

    const uint32_t Get() const { return m_framebuffer; }
    void Bind() const;
    const TexturePtr GetColorAttachment() const { return m_colorAttachment; }

private:
    Framebuffer() {}
    bool InitWithColorAttachment(const TexturePtr colorAttachment);

    uint32_t m_framebuffer { 0 };
    uint32_t m_depthStencilBuffer { 0 };            // Render Buffer 는 Depth,stencil 용
    TexturePtr m_colorAttachment;                   // Color Buffer는 Texture 사용할것 
                                                    // 화면에 보여지는 buffer가 아닌 다른 buffer 에 렌더링하는걸 offscreen rendering 이라고 하는듯
};

#endif // __FRAMEBUFFER_H__
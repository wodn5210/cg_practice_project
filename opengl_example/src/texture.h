#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "image.h"

CLASS_PTR(Texture)
class Texture {
public:
    static TextureUPtr CreateFromImage(const Image* image);     // 왜 스마트포인터 안씀?
                                                                // Uptr이면 소유권이 여기로 넘어옴
                                                                // class 내에 texture 저장하게 만들어서 외부에서 이미지 못쓸 수 있음
                                                                // ㄱ냥 Ptr사용해도 개념은 비슷함
                                                                // 근데 텍스쳐는 glTex2DImag 에서만 이미지사용해서 더이상 필요없어보여서
                                                                // 즉 texture만들때 이미지 한번만 사용하고 더이상 사용안해서 그냥 포인터로사용
    ~Texture();

    const uint32_t Get() const { return m_texture; }
    void Bind() const;
    void SetFilter(uint32_t minFilter, uint32_t magFilter) const;
    void SetWrap(uint32_t sWrap, uint32_t tWrap) const;
    
private:
    Texture() {}
    void CreateTexture();
    void SetTextureFromImage(const Image* image);

    uint32_t m_texture { 0 };
};

#endif // __TEXTURE_H__
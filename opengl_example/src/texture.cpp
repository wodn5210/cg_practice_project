#include "texture.h"

TextureUPtr Texture::CreateFromImage(const Image* image) {
    auto texture = TextureUPtr(new Texture());
    texture->CreateTexture();
    texture->SetTextureFromImage(image);
    return std::move(texture);
}

Texture::~Texture() {
    if (m_texture) {
        glDeleteTextures(1, &m_texture);
    }
}

void Texture::Bind() const {
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

void Texture::SetFilter(uint32_t minFilter, uint32_t magFilter) const {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

// repaet, mirror 등등설정
void Texture::SetWrap(uint32_t sWrap, uint32_t tWrap) const {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
}
    
void Texture::CreateTexture() {
    glGenTextures(1, &m_texture);
    // bind and set default filter and wrap option
    Bind();
/*
    minmap: 작은 크기를 표현하기 위해서 원본이미지의 축소버전을 미리 만들어둔다.
    기본을 0lv 로 해서 9lv 까지 생성함
    각 lv 크기는 가로세로 2씩 나눔
    메모리는 1/3추가 소모
*/    
    // nearest_minmap_neareast: 적합한 레벨의 texture level 선택한뒤 nearest neighbor 픽셀 선택

    // 민맵중에 적당한 level 2개를 선택해서 interpolation 한뒤 다시 interpolation한다.
    SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

void Texture::SetTextureFromImage(const Image* image) {
    GLenum format = GL_RGBA;
    switch (image->GetChannelCount()) {
        default: break;
        case 1: format = GL_RED; break;
        case 2: format = GL_RG; break;
        case 3: format = GL_RGB; break;
    }

    m_width = image->GetWidth();
    m_height = image->GetHeight();
    m_format = format;

    // A없는 이미지로 사용하면 그냥 255로 채워짐
    glTexImage2D(GL_TEXTURE_2D, 0, m_format,
                 m_width, m_height, 0,
                 format, GL_UNSIGNED_BYTE,
                 image->GetData());

    // minmap 알아서 생성해준다.
    glGenerateMipmap(GL_TEXTURE_2D);        
}



TextureUPtr Texture::Create(int width, int height, uint32_t format) {
    auto texture = TextureUPtr(new Texture());
    texture->CreateTexture();
    texture->SetTextureFormat(width, height, format);
    texture->SetFilter(GL_LINEAR, GL_LINEAR);
    return std::move(texture);
}

void Texture::SetTextureFormat(int width, int height, uint32_t format) {
    m_width = width;
    m_height = height;
    m_format = format;

    glTexImage2D(GL_TEXTURE_2D, 0, m_format,
                 m_width, m_height, 0,
                 m_format, GL_UNSIGNED_BYTE,
                 nullptr);                      // texture 가 저장될 데이터공간만 확보. 어떤 데이터가 들어가진 않음
}
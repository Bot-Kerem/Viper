#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glad/glad.h>
#include <util/globals/global.hpp>
#include "sprite2d.hpp"

namespace Viper::Renderer {
    Sprite2D::Sprite2D() {
        
    };
    Sprite2D::Sprite2D( const std::string& path, int width, int height ) : Path(path), Width( width ), Height( height ) {
        glGenTextures(1, &SpriteID);
        glBindTexture(GL_TEXTURE_2D, SpriteID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        int m_TexWidth, m_TexHeight, m_TexChannels;
        unsigned char* m_TextureData = stbi_load(path.c_str( ), &m_TexWidth, &m_TexHeight, NULL, 4);
        if( m_TextureData == nullptr )
            printf("gl_BindFrameBufferTexture nullptr.\n");
            //Globals::ConsoleContext::AddLog( VIPER_ICON_ERR " Error loading texture!", VIPER_FORMAT_STRING("Missing texture.\nNo such file or dir: %s!",
            //path.c_str( ) ), Globals::ConsoleError );

        if( m_TextureData ) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_TexWidth, m_TexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_TextureData);
            glGenerateMipmap(GL_TEXTURE_2D);
        };

        stbi_image_free(m_TextureData);
        glBindTexture(GL_TEXTURE_2D, 0);
     };

    Sprite2D::Sprite2D( int width, int height ) {
        glGenTextures(1, &SpriteID);
        glBindTexture(GL_TEXTURE_2D, SpriteID);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
       
        glBindTexture(GL_TEXTURE_2D, 0);

        Width = width;
        Height = height;
    };
    
    Sprite2D::~Sprite2D() {
        Delete();
        //glDeleteTextures(1, &SpriteID);
    };

    void Sprite2D::Delete() {
        glDeleteTextures(1, &SpriteID);
    };

    Ref< Sprite2D > Sprite2D::Create( const std::string& sprite_path ) {
        return CreateRef< Sprite2D >( sprite_path, 1, 1 );
    };

    Ref< Sprite2D > Sprite2D::Create( const std::string& sprite_path, int width, int height ) {
        return CreateRef< Sprite2D >( sprite_path, width, height );
    };

    Ref< Sprite2D > Sprite2D::Create( int width, int height ) {
        return CreateRef< Sprite2D >( width, height );
    };

    uint32_t Sprite2D::GetSprite() const {
        return SpriteID;
    };

    void Sprite2D::Bind() {
        glBindTexture(GL_TEXTURE_2D, SpriteID);
    };

    void Sprite2D::SetData( void* data, uint32_t size ) {
        
        glTextureSubImage2D(SpriteID, 0, 0, 0, Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    };

    void Sprite2D::Unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    };

    void Sprite2D::Bind( uint32_t slot ) {
        glBindTextureUnit( slot, SpriteID );
    };

    void Sprite2D::Change( const std::string& location ) {
        Bind();
        Path = location;
        int m_TexWidth, m_TexHeight, m_TexChannels;
        unsigned char* m_TextureData = stbi_load(location.c_str( ), &m_TexWidth, &m_TexHeight, NULL, 4);

        if( m_TextureData ) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_TexWidth, m_TexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_TextureData);
            glGenerateMipmap(GL_TEXTURE_2D);
        };

        stbi_image_free(m_TextureData);
        Unbind();
    };

    std::string Sprite2D::GetCurrentPath() {
        return Path;
    };
};
#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <graphics/renderer/sprite2d.hpp>
#include <util/ref/reference.hpp>
#include <string.h>

#define VIPER_INCOMPLETE_COMP( CompDef ) struct CompDef { \
    CompDef() { printf("error, comp not complete for " #CompDef ); } \
    CompDef( const CompDef& o ) = default; \
    \
};

namespace Viper {
    struct TransformComponent {
        TransformComponent() = default;
        TransformComponent( const TransformComponent& o ) = default;
        TransformComponent( const glm::vec3& pos ) : position( pos ), scale(glm::vec3(0.0f)), rotation(glm::vec3(0.0f)) { };
        TransformComponent( const glm::vec3& pos, const glm::vec3& size ) : position( pos ), scale(size), rotation(glm::vec3(0.0f)) { };
        TransformComponent( const glm::vec3& pos, const glm::vec3& size, const glm::vec3& rot ) : position( pos ), scale(size), rotation(rot) { };
        glm::vec3 position;
        glm::vec3 scale;
        glm::vec3 rotation;
    };

    struct TagComponent {
        TagComponent() = default;
        TagComponent( const TagComponent& o ) = default;
        TagComponent( const std::string& tagname ) : tag( tagname ) { };

        std::string tag;
    };

    struct SpriteRendererComponent {
        SpriteRendererComponent() = default;
        SpriteRendererComponent( const SpriteRendererComponent& o ) = default;
        SpriteRendererComponent( const glm::vec4& col ) : color( col ), sprite_texture( ) { };
        SpriteRendererComponent( const glm::vec4& col, const Ref< Renderer::Sprite2D >& sprite ) : color( col ), sprite_texture( sprite ) { };

        glm::vec4 color;
        Ref< Renderer::Sprite2D > sprite_texture;
        float tiling = 1.0f;
    };

    VIPER_INCOMPLETE_COMP(Rigidbody2DComponent)
    VIPER_INCOMPLETE_COMP(BoxCollider2DComponent)
    VIPER_INCOMPLETE_COMP(CameraComponent)
    VIPER_INCOMPLETE_COMP(NativeScriptComponent)
};
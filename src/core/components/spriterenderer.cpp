#include <ImGui/imgui.h>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include "../util/globals/global.hpp"
#include "../imguieditor/fontawesome5.hpp"
#include "spriterenderer.hpp"

namespace Viper::Components {
    SpriteRenderer::SpriteRenderer() {
        object = nullptr;
        color = glm::vec4(1.0f,1.0f,1.0f,1.0f);
    };

    SpriteRenderer::SpriteRenderer(GameObject* obj ) {
        object = obj;
        color = glm::vec4(1.0f,1.0f,1.0f,1.0f);
    };

    SpriteRenderer::SpriteRenderer(GameObject* obj, glm::vec4 col ) {
        object = obj;
        color = col;
    };

    void SpriteRenderer::Awake() {
    };

    void SpriteRenderer::Update(double deltatime) {
        tr = object->GetComponent< Transform >( );
        Renderer::Renderer2D::DrawQuadRotated(
        glm::vec2(tr.position.x, tr.position.y),
        glm::vec2(tr.scale.x, tr.scale.y),
        tr.rotation.z,
        RendererAPI::Color(color.x, color.y, color.z, color.w));
    };

    void SpriteRenderer::Editor() {
        ImGuiTreeNodeFlags t = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Framed;
        if(ImGui::TreeNodeEx(" " ICON_FA_PAINT_BRUSH " SpriteRenderer", t)) {
            ImGui::ColorEdit4("Sprite Color", glm::value_ptr(color));
            ImGui::TreePop();
        };
    };
};
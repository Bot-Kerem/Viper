#include "hierarchy.hpp"
#include <windows.h>

//#include <graphics/renderer/renderer.hpp>
#include <ImGui/imgui.h>

#include "../fontawesome5.hpp"
#include "../entitycomponents.hpp"
#include "../sceneentity.hpp"
#include "../scene.hpp"

#include <graphics/renderer/renderer2d.hpp>
#include <graphics/renderer/renderer3d.hpp>
#include <util/globals/global.hpp>
#include <graphics/renderer/rendercommand.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace Viper {
    static char buff[80] = { '\0' };
    SceneHierarchy::SceneHierarchy( Scene* SceneContext ) : m_Context( SceneContext ) { };

    void SceneHierarchy::OnImGuiRender( Timestep::Timestep ts, const Ref< FrameBuffer >& framebuffer ) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));

        if (ImGui::Begin(ICON_FA_SITEMAP "  Hierarchy") ) {
            ImGui::Text("engine fps: %.2f, average %.3fms/frame", 1.0f / ts.seconds(), ts.milliseconds());

            //auto stats = Renderer2D::GetStats();
            //ImGui::Text("draw calls: %d", stats.calls_this_frame);
            //ImGui::Text("max verts: %d", stats.max_vertices_allowed);
            //ImGui::Text("max indices: %d", stats.max_indices_allowed);
            //ImGui::Text("max quads: %d", stats.max_quads_allowed);

            auto stats3d = Renderer3D::GetStats();
            ImGui::Text("Cubes: %d", stats3d.m_CubeCount);
            ImGui::Text("IndexCount: %d", stats3d.m_IndexCount);
            ImGui::Text("DrawCalls: %d", stats3d.m_DrawCalls);

            ImGui::DragFloat("Rad", &Globals::Editor::Radians );

            ImGui::Text("Color Attachment.");
            ImGui::Image( reinterpret_cast< ImTextureID >( framebuffer->GetColorAttachment( ) ), ImVec2(100.0f, 100.0f));
            ImGui::Text("Depth Attachment.");
            ImGui::Image( reinterpret_cast< ImTextureID >( framebuffer->GetDepthAttachment( ) ), ImVec2(100.0f, 100.0f));

            if( m_Context->m_box_world != nullptr ) {
                if( ImGui::Button("Stop simulation")) {
                    m_Context->OnPhysicsEnd();
                    m_Context->m_SceneState = SceneStates::State_None;
                }
            } else {
                if( ImGui::Button("Start simulation")) {
                    m_Context->OnPhysicsStart();
                    m_Context->m_SceneState = SceneStates::State_Simulating;
                }
            };


            if(ImGui::Button("Open file dialogue")) {
                OPENFILENAME ofn;
                char fileName[256] = "";
                ZeroMemory(&ofn, sizeof(ofn));

                ofn.lStructSize = sizeof(OPENFILENAME);
                ofn.hwndOwner = NULL;
                ofn.lpstrFilter = "Viper scene (*.viper)\0*.viper\0";
                ofn.lpstrFile = fileName;
                ofn.nMaxFile = MAX_PATH;
                ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
                ofn.lpstrDefExt = "";

                if ( GetOpenFileName(&ofn) ) {
                    printf("%s\n", fileName);
                };
            };
            
            OnImGuiRenderItems( ts );
            if( ImGui::BeginPopupContextWindow(0, 1, false)) {
                if(ImGui::MenuItem("Create Empty Entity"))
                {
                    auto ent = m_Context->CreateEntity( "hentai maker" );
                    Entity entity = { ent, m_Context };
                    ent = entity;
                    m_Context->m_selected_entity = ent;
                }
                ImGui::EndPopup();
            };
            ImGui::End();
        };
        ImGui::PopStyleVar();
    };

    void SceneHierarchy::OnImGuiRenderItems( Timestep::Timestep ts ) {
        auto view = m_Context->m_register.view< TagComponent >( );

        for( auto entity : view ) {

            auto tag_component = view.get< TagComponent >( entity );

            ImGuiTreeNodeFlags flag = ( ( m_Context->m_selected_entity == entity ) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
            bool is_open = ImGui::TreeNodeEx( tag_component.tag.c_str( ), flag, tag_component.tag.c_str( ) );

            if (ImGui::IsItemClicked())
                m_Context->m_selected_entity = entity;

            if (is_open) {
                if (ImGui::Button(std::string(ICON_FA_TRASH_ALT " Remove ").append(tag_component.tag).c_str())) {
                    m_Context->DestroyEntity( entity );
                    m_Context->m_selected_entity = entt::null;
                }
                ImGui::TreePop();
            };
        };
    };
};
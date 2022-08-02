#include "inspector.hpp"

#include <imguieditor/fontawesome5.hpp>
#include <ImGui/imgui.h>

#include <scene/entitycomponents.hpp>
#include <scene/sceneentity.hpp>
#include <scene/scene.hpp>

#include <glm/gtc/type_ptr.hpp>

static void imgui_gizmo_transform(const std::string& string, glm::vec3& values, float reset_value = 0.0f) {
    ImGui::Columns(2, std::string("##").append(string).c_str( ), false);
    ImGui::SetColumnWidth(0, 70.0f);
    ImGui::Text( string.c_str( ) );
    ImGui::NextColumn();
    
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(230,53,97));
    if( ImGui::Button("X"))
        values.x = reset_value;
    ImGui::PopStyleColor(1);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(60.0f);
    ImGui::DragFloat(std::string( "##" ).append( string ).append( "##X" ).c_str( ), &values.x, 0.1f);

    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(37,206,113));
    if( ImGui::Button("Y"))
        values.y = reset_value;
    ImGui::PopStyleColor(1);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(60.0f);
    ImGui::DragFloat(std::string( "##" ).append( string ).append( "##Y" ).c_str( ), &values.y, 0.1f);

    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(41,105,169));
    if( ImGui::Button("Z"))
        values.z = reset_value;
    ImGui::PopStyleColor(1);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(60.0f);
    ImGui::DragFloat(std::string( "##" ).append( string ).append( "##Z" ).c_str( ), &values.z, 0.1f);
    ImGui::PopStyleVar(1);
    ImGui::Columns(1);
};

namespace Viper {
    SceneInspector::SceneInspector( Scene* SceneContext ) : m_Context( SceneContext ) { };
    static std::string m_Nullified = "";
    void SceneInspector::OnImGuiPopulateComponents( Entity entity ) {
        if( ImGui::MenuItem("SpriteRenderer") && !entity.has< SpriteRendererComponent >( ) ) {
            entity.add< SpriteRendererComponent >( glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
        };
    };

    void SceneInspector::OnImGuiRender( Timestep::Timestep ts ) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
        if (ImGui::Begin(ICON_FA_CUBE "  Inspector")) {
            if( m_Context->m_selected_entity != entt::null ) {
                ImGuiTreeNodeFlags t = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Framed;
                
                Entity ent = { m_Context->m_selected_entity, m_Context };

                if( ent.has< TransformComponent >( ) ) {
                    auto name = ent.get< TagComponent >( );
                    ImGui::Text("Component name: %s", name.tag.c_str( ) );
                };

                if( ent.has< TransformComponent >( ) ) {
                    auto &[pos, scale, rot] = ent.get< TransformComponent >( );

                    if(ImGui::TreeNodeEx( " " ICON_FA_CUBES "  Transform", t)) {
                        imgui_gizmo_transform("Position", pos);
                        imgui_gizmo_transform("Rotation", rot);
                        imgui_gizmo_transform("Scale", scale, 1.0f);
                        
                        ImGui::TreePop();
                    };
                };

                if( ent.has< SpriteRendererComponent >( ) ) {
                    auto& sprite = ent.get< SpriteRendererComponent >( );
                    ImGui::Button("DragDrop Target");
                    //if(ImGui::BeginDragDropTarget()) {
                    //    if( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_EXPLORER_DRAG_DROP") ) {
                    //        const char* sz_output = ( const char* )payload->Data;
                    //        m_Nullified = sz_output;
                    //    }
                    //    ImGui::EndDragDropTarget();
                    //};

                    if(ImGui::TreeNodeEx(" " ICON_FA_PAINT_BRUSH "  SpriteRenderer", t)) {
                        ImGui::ColorEdit4("Sprite Color", glm::value_ptr(sprite.color));
                        if(sprite.sprite_texture.get() == nullptr) {
                            if(ImGui::Button("Bake Texture")) {
                                sprite.sprite_texture = Renderer::Sprite2D::Create( "resources/textures/checkerboard.png", 64, 64 );
                            };
                        } else {
                            ImGui::DragFloat("Tiling", &sprite.tiling, 0.05f, 1.0f, 100.0f );
                            ImGui::Image( reinterpret_cast< ImTextureID >( sprite.sprite_texture->GetSprite( ) ),
                                ImVec2(64.0f, 64.0f ) 
                            );

                            if(ImGui::BeginDragDropTarget()) {
                                if( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_EXPLORER_DRAG_DROP") ) {
                                    const char* sz_output = ( const char* )payload->Data;
                                    sprite.sprite_texture->Change( sz_output );
                                }
                                ImGui::EndDragDropTarget();
                            };

                            if(ImGui::Button("Delete texture")) {
                                sprite.sprite_texture.reset();
                            }
                        };
                        ImGui::TreePop();
                    };
                };

                if(ImGui::BeginPopupContextWindow("##AddComponent", 1, false)) {
                    OnImGuiPopulateComponents(ent);
                    ImGui::EndPopup();
                };
            };
            ImGui::End();
        };
        ImGui::PopStyleVar();
    };
};
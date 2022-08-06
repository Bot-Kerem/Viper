#include "inspector.hpp"

#include <iostream>
#include <imguieditor/fontawesome5.hpp>
#include <ImGui/imgui.h>

#include <scene/entitycomponents.hpp>
#include <scene/sceneentity.hpp>
#include <scene/scene.hpp>
#include <components/transform.hpp>
#include <components/rigidbody2d.hpp>
#include <components/boxcollision2d.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <nlohmann/json.hpp>
#include <util/jsonfilehandler/jsonfilehandler.hpp>

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

constexpr const char* RigidbodyTypes[] = { "Static", "Dynamic", "Kinematic" };

namespace Viper {
    SceneInspector::SceneInspector( Scene* SceneContext ) : m_Context( SceneContext ) { };
    static std::string m_Nullified = "";

    void SerializeEntity( Entity entity, std::uint32_t entity_id, nlohmann::json& data ) {
        TransformComponent tr;
        SpriteRendererComponent sr;
        Rigidbody2DComponent rb2d;
        BoxCollider2DComponent bc2d;

        if(entity.has<TransformComponent>()) {
            tr = entity.get<TransformComponent>();
            data.push_back({
                entity_id,
                {
                    {"Tag", entity.get< TagComponent >( ).tag.c_str( )},
                    {"Transform", {
                        {"Position", {
                            {"X", tr.position.x},
                            {"Y", tr.position.y},
                            {"Z", tr.position.z}
                        }},
                        {"Scale", {
                            {"X", tr.scale.x},
                            {"Y", tr.scale.y},
                            {"Z", tr.scale.z}
                        }},
                        {"Rotation", {
                            {"X", tr.rotation.x},
                            {"Y", tr.rotation.y},
                            {"Z", tr.rotation.z}
                        }}
                 }},
            }});
        } // End transform

        if(entity.has<SpriteRendererComponent>()) {
            sr = entity.get<SpriteRendererComponent>();
            data.push_back({
                entity_id,
                {
                    {"Tag", entity.get< TagComponent >( ).tag.c_str( )},
                    {"SpriteRenderer", {
                        {"Color", {
                            {"R", sr.color.r},
                            {"G", sr.color.g},
                            {"B", sr.color.b},
                            {"A", sr.color.a}
                        }},
                        {"Sprite", {
                            {"Width", sr.sprite.get()->GetWidth()},
                            {"Height", sr.sprite.get()->GetHeight()},
                            {"Path", sr.sprite.get()->GetPath()},
                            {"SpriteID", sr.sprite.get()->GetSpriteID()},
                            {"Tiling", sr.tiling}
                        }}
                    }}
                }
            });
        } // End spriterenderer

        if(entity.has<Rigidbody2DComponent>()) {
            rb2d = entity.get<Rigidbody2DComponent>();
            data.push_back({
                entity_id,
                {
                    {"Tag", entity.get< TagComponent >( ).tag.c_str( )},
                    {"Rigidbody2D", {
                        {"Type", static_cast<int>(rb2d.Type)},
                        {"FixedRotation", rb2d.FixedRotation}
                    }}
                }
            });
        } // End rigidbody2d 

        if(entity.has<BoxCollider2DComponent>()) {
            bc2d = entity.get<BoxCollider2DComponent>();
            data.push_back({
                entity_id,
                {
                    {"Tag", entity.get< TagComponent >( ).tag.c_str( )},
                    {"BoxCollider2D",{
                        {"Offset", {
                            {"X", bc2d.offset.x},
                            {"Y", bc2d.offset.y}
                        } },
                        {"Size", {
                            {"X", bc2d.size.x},
                            {"Y", bc2d.size.y}
                        }},
                        {"Density", bc2d.density},
                        {"Friction", bc2d.friction},
                        {"Restitution", bc2d.restitution},
                        {"RestitutionThreshold", bc2d.restitutionthreshold}
                    }}
                }
            });
        } 
    }

    void SceneInspector::OnImGuiPopulateComponents( Entity entity ) {
        if( ImGui::MenuItem("SpriteRenderer") && !entity.has< SpriteRendererComponent >( ) ) {
            entity.add< SpriteRendererComponent >( glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
        };

        if( ImGui::MenuItem("Rigidbody2D") && !entity.has< Rigidbody2DComponent >( ) ) {
            entity.add< Rigidbody2DComponent >( );
        };

        if( ImGui::MenuItem("BoxCollider2D") && !entity.has< BoxCollider2DComponent >( ) ) {
            entity.add< BoxCollider2DComponent >( );
        };
    };

    void SceneInspector::OnImGuiRender( Timestep::Timestep ts ) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
        if (ImGui::Begin(ICON_FA_CUBE "  Inspector")) {
            if( m_Context->m_selected_entity != entt::null ) {
                ImGuiTreeNodeFlags t = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Framed;
                
                Entity ent = { m_Context->m_selected_entity, m_Context };
                static char tag_buffer[ 32 ] = { '\0' };
                if( ent.has< TransformComponent >( ) ) {
                    auto& name = ent.get< TagComponent >( );
                    ImGui::Text("Component name: %s", name.tag.c_str( ) );
                    ImGui::InputText("##comp_new_name", tag_buffer, 32 );
                    ImGui::SameLine();
                    if(ImGui::Button("Set name")) {
                        name.tag = tag_buffer;
                        tag_buffer[0] = '\0';
                    };
                };

                if( m_Context->m_SceneState == SceneStates::State_Simulating )
                    ImGui::BeginDisabled();

                if( ent.has< TransformComponent >( ) ) {
                    auto &[pos, scale, rot] = ent.get< TransformComponent >( );

                    if(ImGui::TreeNodeEx( " " ICON_FA_CUBES "  Transform", t)) {
                        imgui_gizmo_transform("Position", pos);
                        imgui_gizmo_transform("Rotation", rot);
                        imgui_gizmo_transform("Scale", scale, 1.0f);
                        
                        ImGui::TreePop();
                    };
                };

                if( ent.has< Rigidbody2DComponent >( ) ) {
                    auto& rb2d = ent.get< Rigidbody2DComponent >( );
                    if(ImGui::TreeNodeEx( " " ICON_FA_CUBES "  Rigidbody2D", t)) {
                        if( ImGui::Button("Change to dynamic"))
                            rb2d.Type = Rigidbody2DComponent::BodyType::body_dynamic;

                        const char* current_body_type = RigidbodyTypes[(int)rb2d.Type];
                        if(ImGui::BeginCombo("Body type", current_body_type)) {
                            for( int i = 0; i < 2; i++ ) {
                                bool is_selected = current_body_type == RigidbodyTypes[i];
                                if( ImGui::Selectable(RigidbodyTypes[i], is_selected)) {
                                    current_body_type = RigidbodyTypes[i];
                                    rb2d.Type = (Rigidbody2DComponent::BodyType)i;
                                };

                                if( is_selected )
                                    ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        };

                        ImGui::Checkbox("Fixed Rotation", &rb2d.FixedRotation);
                        if(ImGui::Button("Add Force")) {
                            b2Body* body = ( b2Body* )rb2d.Rigidbody;
                            auto pos = glm::vec2(0.0f, 1.0f);
                            auto point = glm::vec2(0.0f, 0.0f);

                            body->ApplyForce({ pos.x, pos.y },{0.0f, 0.0f}, true);
                        }
                        ImGui::TreePop();
                    };
                };

                if( ent.has< BoxCollider2DComponent >( ) ) {
                    auto& box2d = ent.get< BoxCollider2DComponent >( );
                    if(ImGui::TreeNodeEx( " " ICON_FA_CUBES "  BoxCollider2D", t)) {
                        ImGui::DragFloat2("Offset", glm::value_ptr(box2d.offset));
                        ImGui::DragFloat2("Size", glm::value_ptr(box2d.size));
                        ImGui::DragFloat("Density", &box2d.density);
                        ImGui::DragFloat("Friction", &box2d.friction);
                        ImGui::DragFloat("Restitution", &box2d.restitution);
                        ImGui::DragFloat("Restitution Threshold", &box2d.restitutionthreshold);
                        ImGui::TreePop();
                    };
                };

                if( ent.has< SpriteRendererComponent >( ) ) {
                    auto& sprite = ent.get< SpriteRendererComponent >( );

                    if(ImGui::TreeNodeEx(" " ICON_FA_PAINT_BRUSH "  SpriteRenderer", t)) {
                        ImGui::ColorEdit4("Sprite Color", glm::value_ptr(sprite.color));
                         
                        if(sprite.sprite.get() == nullptr) {
                            if(ImGui::Button("Bake Texture")) {
                                sprite.sprite = Sprite2D::Create( "resources/textures/checkerboard.png" );
                            };
                        } else {
                            ImGui::DragFloat("Tiling", &sprite.tiling, 0.05f, 1.0f, 100.0f );
                            ImGui::Image( reinterpret_cast< ImTextureID >( sprite.sprite->GetSprite( ) ),
                                ImVec2(64.0f, 64.0f ) 
                            );

                            if(ImGui::BeginDragDropTarget()) {
                                if( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_EXPLORER_DRAG_DROP") ) {
                                    const char* sz_output = ( const char* )payload->Data;
                                    sprite.sprite->Change( sz_output );
                                }
                                ImGui::EndDragDropTarget();
                            };

                            if(ImGui::Button("Delete texture")) {
                                sprite.sprite.reset();
                            }
                        };
                        
                        ImGui::TreePop();
                    };
                };

                if( m_Context->m_SceneState == SceneStates::State_Simulating )
                    ImGui::EndDisabled();

                if(ImGui::BeginPopupContextWindow("##AddComponent", 1, false)) {
                    OnImGuiPopulateComponents(ent);
                    ImGui::EndPopup();
                };
            };
            ImGui::End();
        };
        ImGui::PopStyleVar();

        ImGui::ShowDemoWindow();

        if( ImGui::BeginMainMenuBar()) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
            if(ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("New", "CTRL+N")) {

                };
                if(ImGui::MenuItem("Save", "CTRL+S")) {
                    //nlohmann::json data = nlohmann::json::array();
                    nlohmann::json data;

                    m_Context->m_register.each([&](auto entity_id ){
                        Entity ent = { entity_id, m_Context };
                            if(!ent) // ent = 0
                                return;
                            SerializeEntity(ent, static_cast<uint32_t>(entity_id), data);
                        });
                        
                    Viper::Util::JSONFileHandler().Write("test.json", data);
                };
                if(ImGui::MenuItem("Open", "CTRL+O")) {

                    // Before we load in a scene we want to clear the scene from all objects first.
                    m_Context->m_register.clear();

                    //nlohmann::json data = Viper::Util::JSONFileHandler().Read("test.json");

                    /*for(int i = 0; i != data.size(); ++i ) {
                        // Make sure we dont hadd the same game object more than once
                        int id = data[i][0].get<int>();
                        auto component = data[i][1];

                        std::vector<std::string> gomNames;
                        static int lastId = -1;
                        if(id != lastId) {
                            lastId = id;
                        }
                    }*/
                };
                ImGui::EndMenu();
            };
            ImGui::PopStyleVar();
            ImGui::EndMainMenuBar();
        };
    };
};
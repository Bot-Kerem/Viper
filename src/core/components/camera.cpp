#include <ImGui/imgui.h>
#include <memory>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include "../util/globals/global.hpp"
#include "../util/input/input.hpp"
#include "../imguieditor/fontawesome5.hpp"
#include "camera.hpp"

namespace Viper::Components {
    Camera::Camera() {
        object = nullptr;
    };

    Camera::Camera(GameObject* parent ) {
        object = parent;
    };

    void Camera::Awake() {
    };

    float Lerp( float a, float b, float t ) {
        return a + ( b - a ) * t;
    };

    void Camera::Update(double deltatime) {
        auto& tr = object->GetComponent< Transform >( );
        float direction_x = 0.0f;
        float direction_y = 0.0f;
        if( Input::Input::IsKeyPressed(68) )
            direction_x = 1.0f;//tr.position.x += 1.0f;

        if( Input::Input::IsKeyPressed(65) )
            direction_x = -1.0f;//tr.position.x -= 1.0f;

        if( Input::Input::IsKeyPressed(87) )
            direction_y = 1.0f;//tr.position.x += 1.0f;

        if( Input::Input::IsKeyPressed(83) )
            direction_y = -1.0f;//tr.position.x -= 1.0f;
        
        tr.position.x += direction_x * 1.0f * deltatime * 3.0f;
        tr.position.y += direction_y * 1.0f * deltatime * 3.0f;
    };

    void Camera::Editor() {
        ImGuiTreeNodeFlags t = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Framed;
        if(ImGui::TreeNodeEx( " " ICON_FA_CUBE " Camera", t)) {
            ImGui::TreePop();
        };
    };
};
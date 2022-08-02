#include <ImGui/imgui.h>
#include <memory>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include "../util/globals/global.hpp"
#include "../util/input/input.hpp"
#include "../imguieditor/fontawesome5.hpp"
#include "rigidbody2d.hpp"

namespace Viper::Components {
    Rigidbody2D::Rigidbody2D() {
        object = nullptr;
        enabled = true;
        m_Force = 100.0f;
        m_Mass = 100.0f;
        grounded = false;
        m_Gravity = 2.0f;
        velocity = glm::vec3( 0.0f, 0.0f, 0.0f );
        boundary = true;
    };

    Rigidbody2D::Rigidbody2D(GameObject* parent ) {
        object = parent;
        enabled = true;
        m_Force = 100.0f;
        m_Mass = 100.0f;
        m_Gravity = 2.0f;
        grounded = false;
        velocity = glm::vec3( 0.0f, 0.0f, 0.0f );
        boundary = true;
    };

    void Rigidbody2D::Update(Timestep::Timestep ts) {
        if(!enabled) {
            velocity = glm::vec3( 0.0f, 0.0f, 0.0f );
            return;
        }
            //return;
        auto& tr = object->GetComponent< Transform >( );

        if( tr.position.y < -30.0f ) {
            tr.position.y = 0.0f;
            velocity = glm::vec3( 0.0f, 0.0f, 0.0f );
        }
        
        tr.position.y += velocity.y * ts;
        velocity.y += -m_Gravity * ts;
    };

    void Rigidbody2D::AddForce( const glm::vec3& force ) {
        velocity = force;
    };
};
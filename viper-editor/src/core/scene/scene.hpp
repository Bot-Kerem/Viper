#pragma once
#include <util/timer/timestep.hpp>
#include <string.h>

#include <scene/panels/hierarchy.hpp>
#include <scene/panels/inspector.hpp>
#include <scene/panels/viewport.hpp>

#include <entt/entt.hpp>
#include <box2d/include/box2d/box2d.h>
#include <viper/viper.hpp>

#include <glm/mat4x4.hpp>

namespace Viper {
    class Entity;

    enum class SceneStates : int {
        State_None,
        State_Playing,
        State_Simulating
    };

    class VIPER_API Scene {
    public:
        Scene();

        ~Scene();
        
        Entity CreateEntity( const std::string& entity_name = "tag_component" );

        void DestroyEntity( entt::entity ent );
        
        void OnUpdate( Timestep::Timestep ts );

        void OnOverlay( Timestep::Timestep ts );

        void OnSetEditorTransform( const glm::mat4& camera_projection, const glm::mat4& camera_transform );

        void OnPhysics();

        void ResetViewport();

        void OnPhysicsStart();

        void OnPhysicsUpdate();

        void OnPhysicsEnd();

        void OnViewportResize( uint32_t width, uint32_t height );

        template< typename... Components >
        Entity GetView();

        Entity GetCameraEntity();

        void CreateCameraEntity();
    private:
        friend class Entity;
        friend class SceneHierarchy;
        friend class SceneInspector;
        friend class SceneViewport;

        Entity GetSelectedEntity();
    private:
        entt::registry m_register;
        entt::entity m_selected_entity = entt::null;

        Scene* m_CopyScene;
        b2World* m_box_world;

        SceneStates m_SceneState = SceneStates::State_None;

        uint32_t m_ViewportWidth = 0;
        uint32_t m_ViewportHeight = 0;

        glm::mat4 default_transform = glm::mat4(1.0f);
        glm::mat4 default_projection = glm::mat4(1.0f);
    };
};
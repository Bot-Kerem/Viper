#pragma once
#include <util/timer/timestep.hpp>
#include <string.h>

#include <imguieditor/scene/editor/hierarchy.hpp>
#include <imguieditor/scene/editor/inspector.hpp>

#include <entt/entt.hpp>
#include <box2d/box2d.h>

namespace Viper {
    class Entity;

    enum class SceneStates : int {
        State_None,
        State_Playing,
        State_Simulating
    };

    class Scene {
    public:
        Scene();

        ~Scene();
        Entity CreateEntity( const std::string& entity_name = "tag_component" );

        void DestroyEntity( entt::entity ent );
        
        void OnUpdate( Timestep::Timestep ts );

        void OnPhysics();

        void ResetViewport();

        void OnPhysicsStart();

        void OnPhysicsUpdate();

        void OnPhysicsEnd();

        template< typename... Components >
        Entity GetView();
    private:
        friend class Entity;
        friend class SceneHierarchy;
        friend class SceneInspector;
    private:
        entt::registry m_register;
        entt::entity m_selected_entity = entt::null;

        Scene* m_CopyScene;
        b2World* m_box_world;

        SceneStates m_SceneState = SceneStates::State_None;
    };
};
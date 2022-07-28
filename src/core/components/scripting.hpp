#pragma once
#include <string.h>
#include "component.hpp"
#include "gameobject.hpp"
#include "../util/ref/reference.hpp"
#include <vector>

namespace Viper::Components {
    class Scripting : public Component {
    public:
        VIPER_COMPONENT_DECLARE(Scripting);
        Scripting();
        Scripting(GameObject* object);


        void Update(double deltatime);

        void Awake();

        void Editor();

        virtual bool Begin();
        virtual void End();
        virtual void OnGui();

        template< typename T = Scripting >
        T& GetComponent();
    protected:
        void ScriptDescriptor( const std::string& scriptname ) { script_name = scriptname; };

        std::string script_name;
        GameObject* parent;
    };

    class TestScript : public Scripting {
    public:
        TestScript();
        TestScript(GameObject* object);

        void OnGui();
    private:
        float m_myValue;
    };
};
#pragma once
#include "../../events/event/event.hpp"
#include "../../util/ref/reference.hpp"
#include "window.hpp"
namespace Viper {
    class ApplicationStartEvent : public Events::Event {
    public:
        VIPER_MAKE_EVENT(ApplicationStart, ApplicationStartEvent);
    };

    class Application {
    public:
        Application();

        void OnEvent( Events::Event& event );
    private:
        Graphics::Window* m_Window;
    };
};
#pragma once

#include <map>
#include <utility>
#include "../util/input/inputhandler/inputhandler.hpp"
#include "../util/input/keycodes.hpp"
#include "../layers/layer/layer.hpp"
#include "../events/event/event.hpp"
#include "../util/globals/global.hpp"
#include "../util/ref/reference.hpp"

namespace Viper::Input {
    //struct KeyboardInputLayerEvent : public Viper::Events::Event {
    //    KeyboardInputLayerEvent() {}
    //    KeyboardInputLayerEvent(std::unordered_map<int, bool> Keyboard) : Keyboard(std::move(Keyboard)) {}
//
    //    // The event sends out an unordered_map of the keyboard each update, int = key, bool = state
    //    std::unordered_map<int, bool> Keyboard;
    //};

    class KeyboardInputLayerEvent : public Events::Event {
    public:
        VIPER_MAKE_EVENT(KeyboardInputLayer, KeyboardInputLayerEvent);
        KeyboardInputLayerEvent(std::unordered_map<int, bool> Keyboard) : Keyboard(std::move(Keyboard)) {}
        std::unordered_map<int, bool> Keyboard;
    };

    class KeyboardInputLayer : public Layers::Layer {
    public:
        KeyboardInputLayer() : Layer("KeyboardInputLayer") {}

        void OnAttach() override {

        }

        void OnDetach() override {

        }

        void OnUpdate() override {
            for(int I = Key::SPACE; I != Key::MENU; ++I) { // First key in the keyboard enum is Space and the last one is Menu
                Keyboard.insert({I, Input::IsKeyPressed(I)});
            }
            //MAKE_REF(KeyboardInputLayerEvent, (Keyboard));

            //Viper::Globals::GlobalsContext::EventHandler->Commit(g_KeyboardInputLayerEvent.get());
            Keyboard.clear(); // Reset the keyboard map when the event has been sent out.
        }

        bool OnKeyboardInputLayerEvent(KeyboardInputLayerEvent& event) {
            // hantera tangentbord.
            return true;
        };

        void OnEvent(Events::Event& event) override {
            Events::EventDispatcher dispatcher(event);

            dispatcher.Dispatch< KeyboardInputLayerEvent >(VIPER_GET_EVENT_FUNC(KeyboardInputLayer::OnKeyboardInputLayerEvent));
        }

    private:
        std::unordered_map<int, bool> Keyboard;
    };
}
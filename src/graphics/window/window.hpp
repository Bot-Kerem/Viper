#pragma once

#include <iostream>
#include <stdio.h>
#include <string>
#include <glfw/glfw3.h>
#include "../../events/event/event.hpp"
#include "../../events/eventhandler/eventhandler.hpp"

namespace Viper::Graphics {
    struct WindowParams_t {
        int Width;            // Window width.
        int Height;           // Window height.
        std::string Title;    // Window title.
        GLFWmonitor *Monitor; // The monitor to use for full screen mode, or `NULL` for windowed mode.
        GLFWwindow *Share;    // The window whose context to share resources with, or `NULL` to not share resources.
        Events::EventBus *EventCallback;
    };

    struct WindowResizeEvent : public Viper::Events::Event {
        WindowResizeEvent(int Width, int Height) : Height(Height), Width(Width) {}

        int Width, Height;
    };

    struct WindowPositionEvent : public Viper::Events::Event {
        WindowPositionEvent(int X, int Y) : X(X), Y(Y) {}

        int X, Y;
    };

    struct WindowCloseEvent : public Viper::Events::Event {};

    class Window {
    public:
        Window(int Width, int Height, const std::string &WindowName);

        ~Window();

        void Initialize();

        void Update() const;

        bool Closed() const;

        static GLFWwindow *CreateWindowEx(WindowParams_t Params);

        void ProcessInput(GLFWwindow *Window);

        static void FramebufferSizeCallback(GLFWwindow *Window, int Width, int Height);

        inline GLFWwindow *Ctx() { return Context; }

        void OnWindowResizeEvent(WindowResizeEvent *E);

        void OnWindowPositionEvent(WindowPositionEvent *E);

        void OnWindowCloseEvent(WindowCloseEvent *E);

    private:
        WindowParams_t WindowParams;

        GLFWwindow *Context;

        Events::EventBus *WindowEvents;

        bool isClosed;

        float AspectRatio;
    };
}
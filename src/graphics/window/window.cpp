// Copyright (c) 2022.

#include <iostream>
#include <string>
#include <functional>
#include <memory>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <spdlog/spdlog.h>
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>
#include "window.hpp"
#include "../../util/input/inputhandler/inputhandler.hpp"
#include "../../imguieditor/imguieditor.hpp"
#include "../../imguieditor/scene/scene.hpp"
#include "../../util/input/mouse/mouseevents.hpp"
#include "../../layers/layer/layer.hpp"
#include "../../layers/layerstack/layerstack.hpp"
#include "../../util/globals/global.hpp"

namespace Viper::Graphics {

    Window::Window(int Width, int Height, const std::string &WindowName) {
        glfwInit();

        WindowParams = {
                Width,
                Height,
                WindowName,
                nullptr,
                nullptr};

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        Context = CreateWindowEx(WindowParams);
        if (!Context) {
            spdlog::error("Failed to create GLFW window");
            glfwTerminate();
        }

        glfwMakeContextCurrent(Context);
        glfwSwapInterval(1); // Vsync on for now.

        glfwSetWindowUserPointer(Context, &WindowParams);

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
            spdlog::error("Failed to initialize GLAD");
        }

        LayerStack = new Viper::Layers::LayerStack();
        Globals::CreateEventHandlerContext();

        SetEventSubscriptions();
        UpdateWindowEvents();

        PushLayer(new Viper::ImGuiEditor(this));
        PushLayer(new Viper::Scene::Scene(this));

        while (!glfwWindowShouldClose(Context)) {
            ProcessInput(Context);

            Globals::Renderer2D->BindFramebuffer();
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            Globals::Renderer2D->UnbindFramebuffer();

            for (auto Layer: *LayerStack) {
                spdlog::info("Updating Layer {0}", Layer->GetLayerName());
                Layer->OnUpdate();
            }

            auto scrolldelta = Input::Input::GetScrollInput();
            auto mouse = Input::MouseEvents::GetMousePosition();

            spdlog::info("MouseCallbackPos: {0}, {1}", mouse.first, mouse.second);

            Update();
        }

        delete LayerStack;

        glfwDestroyWindow(Context);
        glfwTerminate();
    }

    void Window::ProcessInput(GLFWwindow *Window) {
        // if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        // glfwSetWindowShouldClose(Window, true);
    }

    void Window::SetEventSubscriptions() {
        // WindowEvents subscriptions.
        Globals::EventHandler->Subscribe(this, &Window::OnWindowFrameBufferSizeEvent);
        Globals::EventHandler->Subscribe(this, &Window::OnWindowResizeEvent);
        Globals::EventHandler->Subscribe(this, &Window::OnWindowPositionEvent);
        Globals::EventHandler->Subscribe(this, &Window::OnWindowContentScaleEvent);
        Globals::EventHandler->Subscribe(this, &Window::OnWindowMaximizationEvent);
        Globals::EventHandler->Subscribe(this, &Window::OnWindowFocusEvent);
        Globals::EventHandler->Subscribe(this, &Window::OnWindowCloseEvent);
        Globals::EventHandler->Subscribe(new Viper::Input::MouseEvents(),
                                         &Input::MouseEvents::OnMouseCursorPositionEvent);

        for (auto It = LayerStack->end(); It != LayerStack->begin();) {
            Globals::EventHandler->Subscribe((*--It), &Viper::Layers::Layer::OnEvent);
        }
    }

    void Window::UpdateWindowEvents() {
        glfwSetFramebufferSizeCallback(Context, [](GLFWwindow *Window, int Width, int Height) {
            Globals::EventHandler->Commit(new WindowFrameBufferSizeEvent(Width, Height));
        });

        glfwSetWindowSizeCallback(Context, [](GLFWwindow *Window, int Width, int Height) {
            Globals::EventHandler->Commit(new WindowResizeEvent(Width, Height));
        });

        glfwSetWindowPosCallback(Context, [](GLFWwindow *Window, int X, int Y) {
            Globals::EventHandler->Commit(new WindowPositionEvent(X, Y));
        });

        glfwSetWindowContentScaleCallback(Context, [](GLFWwindow *Window, float XScale, float YScale) {
            Globals::EventHandler->Commit(new WindowContentScaleEvent(XScale, YScale));
        });

        glfwSetWindowMaximizeCallback(Context, [](GLFWwindow *Window, int Maximized) {
            Globals::EventHandler->Commit(new WindowMaximizationEvent(Maximized));
        });

        glfwSetWindowFocusCallback(Context, [](GLFWwindow *Window, int Focused) {
            Globals::EventHandler->Commit(new WindowFocusEvent(Focused));
        });

        glfwSetWindowCloseCallback(Context, [](GLFWwindow *Window) {
            Globals::EventHandler->Commit(new WindowCloseEvent());
        });
    }

    void Window::Update() const {
        glfwSwapBuffers(Context);
        glfwPollEvents();
        Input::Input::ResetScroll();
    }

    void Window::PushLayer(Layers::Layer *Layer) {
        LayerStack->PushLayer(Layer);
    }

    void Window::PushOverlay(Layers::Layer *Overlay) {
        LayerStack->PushOverlay(Overlay);
    }

    bool Window::Closed() const {
        return glfwWindowShouldClose(Context);
    }

    GLFWwindow *Window::CreateWindowEx(WindowParams_t Params) {
        return glfwCreateWindow(Params.Width, Params.Height, Params.Title.c_str(), Params.Monitor, Params.Share);
    }

    void Window::OnWindowFrameBufferSizeEvent(WindowFrameBufferSizeEvent *E) {
        spdlog::info("WindowFrameBufferSize Event triggered! Updated viewport to: {0}x{1}", E->Width, E->Height);
        glViewport(0, 0, E->Width, E->Height);
        WindowParams.Width = E->Width;
        WindowParams.Height = E->Height;
        Globals::Renderer2D->ResizeFBO(E->Width, E->Height);
    }

    void Window::OnWindowResizeEvent(WindowResizeEvent *E) {
        spdlog::info("WindowResize Event triggered! New size is {0}x{1}", E->Width, E->Height);
        WindowParams.Width = E->Width;
        WindowParams.Height = E->Height;
        Globals::Renderer2D->ResizeFBO(E->Width, E->Height);
    }

    void Window::OnWindowPositionEvent(WindowPositionEvent *E) {
        spdlog::info("WindowPosition Event triggered! New position is X: {0} : Y: {1}", E->X, E->Y);
    }

    void Window::OnWindowContentScaleEvent(WindowContentScaleEvent *E) {
        spdlog::info("WindowContentScale Event triggered! Scale (X:Y): {0}:{1}", E->XScale, E->YScale);
    }

    void Window::OnWindowMaximizationEvent(WindowMaximizationEvent *E) {
        spdlog::info("WindowMaximization Event triggered! Value: {0}", E->Maximized);
    }

    void Window::OnWindowFocusEvent(WindowFocusEvent *E) {
        spdlog::info("WindowFocus Event triggered! Focus: {0}", E->Focused);
    }

    void Window::OnWindowCloseEvent(WindowCloseEvent *E) {
        spdlog::info("WindowCloseEvent Event triggered!");
        Globals::DestroyEventHandlerContext();
    }
}
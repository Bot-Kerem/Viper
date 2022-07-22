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
#include "../../graphics/shaders/shader/shader.hpp"
#include "../../util/input/inputhandler/inputhandler.hpp"

namespace Viper::Graphics {
    void Window::FramebufferSizeCallback(GLFWwindow *Window, int Width, int Height) {
        glViewport(0, 0, Width, Height);
    }

    Window::Window(int Width, int Height, const std::string &WindowName) {
        glfwInit();

        WindowEvents = new Events::EventBus();

        WindowParams = {
                Width,
                Height,
                WindowName,
                nullptr,
                nullptr,
                this->WindowEvents};

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        Context = CreateWindow(WindowParams);

        if (!Context) {
            spdlog::error("Failed to create GLFW window");
            glfwTerminate();
        }

        glfwMakeContextCurrent(Context);
        glfwSetFramebufferSizeCallback(Context, FramebufferSizeCallback);
        glfwSwapInterval(1); // Vsync on for now.

        glfwSetWindowUserPointer(Context, &WindowParams);

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
            spdlog::error("Failed to initialize GLAD");
        }

        // WindowEvents subscriptions.
        WindowEvents->Subscribe(this, &Window::OnWindowResizeEvent);
        WindowEvents->Subscribe(this, &Window::OnWindowPositionEvent);
        WindowEvents->Subscribe(this, &Window::OnWindowCloseEvent);

        glfwSetWindowSizeCallback(Context, [](GLFWwindow *Window, int Width, int Height) {
            WindowParams_t& WindowData = *(WindowParams_t*)glfwGetWindowUserPointer(Window);
            WindowData.EventCallback->Commit(new WindowResizeEvent(Width, Height));
        });

        glfwSetWindowPosCallback(Context, [](GLFWwindow *Window, int X, int Y) {
            WindowParams_t& WindowData = *(WindowParams_t*)glfwGetWindowUserPointer(Window);
            WindowData.EventCallback->Commit(new WindowPositionEvent(X, Y));
        });

        glfwSetWindowCloseCallback(Context, [](GLFWwindow *Window) {
            WindowParams_t& WindowData = *(WindowParams_t*)glfwGetWindowUserPointer(Window);
            WindowData.EventCallback->Commit(new WindowCloseEvent());
        });

        Shader Shader("resources/test.vert", "resources/test.frag");

        float vertices[] = {
                0.5f, 0.5f, 0.0f,   // top right
                0.5f, -0.5f, 0.0f,  // bottom right
                -0.5f, -0.5f, 0.0f, // bottom left
                -0.5f, 0.5f, 0.0f   // top left
        };
        unsigned int indices[] = {
                // note that we start from 0!
                0, 1, 3, // first Triangle
                1, 2, 3  // second Triangle
        };

        unsigned int VBO, VAO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        while (!glfwWindowShouldClose(Context)) {
            ProcessInput(Context);

            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            Shader.Use();
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

            Update();
        }
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    GLFWwindow *Window::CreateWindow(WindowParams_t Params) {
        return glfwCreateWindow(Params.Width, Params.Height, Params.Title.c_str(), Params.Monitor, Params.Share);
    }

    void Window::ProcessInput(GLFWwindow *Window) {
        // if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        // glfwSetWindowShouldClose(Window, true);
    }

    void Window::Update() const {
        glfwSwapBuffers(Context);
        glfwPollEvents();
    }

    bool Window::Closed() const {
        return glfwWindowShouldClose(Context);
    }

    void Window::OnWindowResizeEvent(WindowResizeEvent *E) {
        spdlog::info("Window size change detected! New size is {0}x{1}", E->Width, E->Height);
    }

    void Window::OnWindowPositionEvent(WindowPositionEvent *E) {
        spdlog::info("Window Position change detected! New position is X: {0} : Y: {1}", E->X, E->Y);
    }

    void Window::OnWindowCloseEvent(WindowCloseEvent *E) {
        spdlog::info("WindowCloseEvent() Called!");
        delete WindowEvents;
        glfwDestroyWindow(Context);
        glfwTerminate();
    }
}
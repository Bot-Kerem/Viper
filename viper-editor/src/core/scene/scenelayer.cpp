#include "scenelayer.hpp"

// utils:
#include <util/globals/global.hpp>
#include <util/timer/timestep.hpp>

// base:
#include <viper/base.hpp>

// scene:
#include "fontawesome5.hpp"
#include "entitycomponents.hpp"
#include "sceneentity.hpp"

// ImGui:
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>
#include <ImGui/ImGuizmo.h>

// renderer:
#include <graphics/renderer/rendercommand.hpp>
#include <graphics/renderer/renderer2d.hpp>
#include <graphics/renderer/renderer3d.hpp>

namespace Viper {
    Ref< Sprite2D > m_Texture;
    SceneLayer::SceneLayer(void* context) : Layer("Scene"), WindowContext((GLFWwindow*)context) {
        //m_Camera = new OrthoGraphicCameraController(1280.0f/720.0f);

        //Renderer2D::Init();
        Renderer3D::Init();

        m_Texture = Sprite2D::Create( "resources/textures/checkerboard.png" );


        Globals::ConsoleContext::AddLog( VIPER_ICON_SUCC " Success!", "Window has been loaded!", Globals::ConsoleSuccess );
        
        m_ActiveScene = CreateRef< Scene >( );
        FrameBuffer::FramebufferSpec_t specification;
        specification.width = 1280;
        specification.height = 720;
        specification.samplers = 1;
        m_FrameBuffer = FrameBuffer::Create( specification );

        m_ActiveScene->CreateCameraEntity();

        m_Viewport = SceneViewport( m_ActiveScene.get(), context );
        m_Hierarchy = SceneHierarchy( m_ActiveScene.get() );
        m_Inspector = SceneInspector( m_ActiveScene.get() );
        m_Filexplorer = SceneFilexplorer();

        OnImGuiInit();
    };

    SceneLayer::~SceneLayer() {
        Destroy();
    };

    void SceneLayer::Destroy() {
        //delete m_Camera;
        //Renderer2D::Shutdown();
        Renderer3D::Shutdown();

        OnImGuiExit();
    };

    void SceneLayer::OnUpdate(Timestep::Timestep ts) {
        Graphics::WindowParams_t &WindowData = *(Graphics::WindowParams_t *)glfwGetWindowUserPointer(WindowContext);
        
        auto window_size = m_Viewport.GetViewportSize();
        if( auto spec = m_FrameBuffer->GetSpecification();
            window_size.x > 0.0f && window_size.y > 0.0f && spec.width != window_size.x || spec.height != window_size.y ) {
                m_FrameBuffer->Resize((uint32_t)window_size.x, (uint32_t)window_size.y);
                m_ActiveScene->OnViewportResize((uint32_t)window_size.x, (uint32_t)window_size.y);
            };
            
        m_FrameBuffer->Bind();
             
        RenderCommand::SetColor({ 0.05f, 0.05f, 0.05f, 1.0f });
        RenderCommand::Clear();

        m_ActiveScene->OnUpdate(ts);

        m_FrameBuffer->Unbind();

        OnImGuiRender(ts);
    }

    void SceneLayer::OnEvent(Events::Event& event) { };

    void SceneLayer::OnImGuiInit() {
        ImGui::CreateContext();
            ImGuiIO &io = ImGui::GetIO();
            ImGuiStyle &style = ImGui::GetStyle();
            ImGui::StyleColorsDark();

            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            io.Fonts->AddFontFromFileTTF("resources/assets/fonts/OpenSans-Bold.ttf", 16.0f);
            //io.Fonts->AddFontDefault();

            static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
            ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
            io.Fonts->AddFontFromFileTTF( FONT_ICON_FILE_NAME_FAS, 12.0f, &icons_config, icons_ranges );

            style.Colors[ImGuiCol_Text] = ImColor( 200, 200, 200, 255 );
            style.Colors[ImGuiCol_Button] = ImColor(35, 35, 35, 255);
            style.Colors[ImGuiCol_ButtonHovered] = ImColor(40, 40, 40, 255);
            style.Colors[ImGuiCol_ButtonActive] = ImColor(45, 45, 45, 255);

            style.Colors[ImGuiCol_FrameBg] = ImColor(14, 16, 39, 255);
            style.Colors[ImGuiCol_FrameBgHovered] = ImColor(17, 19, 47, 255);
            style.Colors[ImGuiCol_FrameBgActive] = ImColor(22, 25, 52, 255);

            style.Colors[ImGuiCol_Tab] = ImColor(17, 11, 44, 255);
            style.Colors[ImGuiCol_TabHovered] = ImColor(22, 17, 68, 255);
            style.Colors[ImGuiCol_TabActive] = ImColor(22, 17, 68, 255);
            style.Colors[ImGuiCol_TabUnfocused] = ImColor(17, 11, 44, 255);
            style.Colors[ImGuiCol_TabUnfocusedActive] = ImColor(41, 29, 93, 255);

            style.Colors[ImGuiCol_Header] = ImColor(26, 16, 61, 255);
            style.Colors[ImGuiCol_HeaderHovered] = ImColor(37, 28, 68, 255);
            style.Colors[ImGuiCol_HeaderActive] = ImColor(36, 24, 83, 255);

            style.Colors[ImGuiCol_SliderGrab] = ImColor(50, 50, 50, 255);
            style.Colors[ImGuiCol_SliderGrabActive] = ImColor(50, 50, 50, 255);

            style.Colors[ImGuiCol_WindowBg] = ImColor(3, 3, 13, 255 );
            style.Colors[ImGuiCol_TitleBg] = ImColor(11, 9, 35, 255 );
            style.Colors[ImGuiCol_TitleBgActive] = ImColor(21, 18, 55, 255 );
            style.Colors[ImGuiCol_TitleBgCollapsed] = ImColor(21, 18, 55, 255 );

            style.Colors[ImGuiCol_Button] = ImColor(35, 27, 75, 255 );
            style.Colors[ImGuiCol_ButtonHovered] = ImColor(35, 27, 75, 255 );
            style.Colors[ImGuiCol_ButtonActive] = ImColor(34, 24, 81, 255 );

            style.Colors[ImGuiCol_SliderGrab] = ImColor( 50, 46, 117, 255 );
            style.Colors[ImGuiCol_SliderGrabActive] = ImColor( 65, 61, 139, 255 );

            style.Colors[ImGuiCol_ScrollbarGrab] = ImColor( 50, 46, 117, 255 );
            style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor( 65, 61, 139, 255 );
            style.Colors[ImGuiCol_ScrollbarGrabActive] = ImColor( 65, 61, 139, 255 );

            style.WindowBorderSize = 0;
            style.FrameBorderSize = 0;
            style.IndentSpacing = 0.0f;
            style.WindowTitleAlign = ImVec2( 0.5f, 0.5f );
            style.WindowPadding = ImVec2( 0, 8 );
            style.IndentSpacing = 10.0f;
            style.ScrollbarRounding = 2.0f;
            style.ScrollbarSize = 10.0f;

            ImGui_ImplGlfw_InitForOpenGL(WindowContext, true);
            ImGui_ImplOpenGL3_Init("#version 460");

            style.GrabRounding = 2.0f;
    };

    void SceneLayer::OnImGuiExit() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    };

    void SceneLayer::OnImGuiBegin() {
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
    };

    void SceneLayer::OnImGuiEnd() {
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    };

    void SceneLayer::OnImGuiRender(Timestep::Timestep ts) {
        OnImGuiBegin();
            m_Viewport.OnImGuiRender(ts, m_FrameBuffer);
            m_Filexplorer.OnImGuiRender(ts);
            m_Hierarchy.OnImGuiRender(ts, m_FrameBuffer);
            m_Inspector.OnImGuiRender(ts);
        OnImGuiEnd();
    };
};
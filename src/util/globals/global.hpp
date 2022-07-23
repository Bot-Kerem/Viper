#pragma once
#include "../../graphics/renderer/renderer.hpp"

namespace Viper::Globals {
    void CreateContext();
    void DestroyContext();

    void CreateRenderingContext();
    void DestroyRenderingContext();

    extern Renderer::Renderer2D* Renderer2D;
};
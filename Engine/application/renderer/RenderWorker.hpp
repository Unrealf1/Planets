#pragma once

#include "application/renderer/RenderState.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <imgui.h>
#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>


class RenderWorker {
public:
    RenderWorker(RendererState& state) : _state(state) {
        _logger = spdlog::stdout_color_st("render worker");
        _logger->set_level(state.params.log_level);
        _logger->info("Render worker is created!");
    }

    void Start();

private:
    void InitWindow();

    void ResetFrame();

    bool _gl_initialized = false;
    RendererState& _state;
    std::shared_ptr<spdlog::logger> _logger;

    void AwaitFrameStart();

    bool AwaitRequest();

    void ProcessRequest(RendererRequest& request);

    void ProcessRequests();

    void Init();
};

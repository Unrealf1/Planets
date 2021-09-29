#pragma once

#include "render/IDrawable.hpp"
#include "application/ApplicationParameters.hpp"

#include <memory>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <GLFW/glfw3.h>


struct RendererRequest {
    enum type {
        draw, init
    } type;

    std::shared_ptr<IDrawable> drawable;
};

struct RendererState {
    std::deque<RendererRequest> requests;
    std::mutex state_mutex;
    
    uint64_t active_frame{0}; //!< Frame that is being updated
    uint64_t draw_frame{0}; //!< Frame that is being rendered
    bool alive{true};
    bool frame_ready;
    
    std::condition_variable rendering_finished;
    std::condition_variable rendering_started;

    DrawInfo draw_info;
    const ApplicationParameters& params;

    GLFWwindow* window;
    bool initialized;
};
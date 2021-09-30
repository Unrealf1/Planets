#pragma once

#include "application/ApplicationParameters.hpp"
#include "application/renderer/RenderState.hpp"
#include "application/renderer/RenderWorker.hpp"

#include <thread>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>


class Renderer {
public:
    Renderer(const ApplicationParameters& params)
    :
    _state{
        .requests={},
        .state_mutex={},
        .active_frame=0, 
        .alive=true,
        .frame_ready=false,
        .rendering_finished={},
        .rendering_started={},
        .draw_info={},
        .params=params,
        .window=nullptr,
        .initialized=false
        
    } {
        _worker = std::thread([this]{
            RenderWorker worker(_state);
            worker.Start();
        });

        _logger = spdlog::stdout_color_st("renderer");
        _logger->set_level(_state.params.log_level);
        _logger->info("Renderer is created!");
    }

    ~Renderer() {
        _state.state_mutex.lock();
        _state.alive = false;
        _state.state_mutex.unlock();

        _state.rendering_finished.notify_one();
        _state.rendering_started.notify_one();

        _worker.join();
    }

    //! Thread-safe. Will add request to the queue
    void AddRequest(RendererRequest request) {
        //_logger->debug("adding request of type {}", request.type);
        std::lock_guard lock(_state.state_mutex);
        _state.requests.push_back(request);
        _state.rendering_started.notify_one();
        _logger->debug("queue size is {}", _state.requests.size());
        if (_state.active_frame > _state.draw_frame) {
            _logger->error("somehting is wrong!");
            exit(0);
        }
    }

    void FinishFrame() {
        _state.state_mutex.lock();
        ++_state.active_frame;
        _state.frame_ready = false;
        _state.state_mutex.unlock();

        _logger->debug("current draw frame: {}, active frame: {}", _state.draw_frame, _state.active_frame);
        _state.rendering_started.notify_one();
    }

    //! Thread-safe. Will block until all requests are satisfied
    void Await() {
        // TODO: implement something less stupid, i don't like this
        _logger->debug("await in: current draw frame: {}, active frame: {}", _state.draw_frame, _state.active_frame);
        std::unique_lock lock(_state.state_mutex);
        _state.rendering_finished.wait(lock, [this]{ return _state.active_frame == _state.draw_frame; });
        _logger->debug("await out: current draw frame: {}, active frame: {}", _state.draw_frame, _state.active_frame);
    }

    //! This should be called once per frame and finish _before_ 
    //! any new requests are added!
    void StartFrame(const DrawInfo& info) {
        _state.draw_info = info;
    }

    GLFWwindow* getWindow() {
        // I'm not sure if lock here is necessary
        return _state.window;
    }

    bool IsInitialized() {
        std::lock_guard lock(_state.state_mutex);
        return _state.initialized;
    }

private:
    RendererState _state;
    std::thread _worker;
    std::shared_ptr<spdlog::logger> _logger;
};

#pragma once

#include "render/IDrawable.hpp"

#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "application/ApplicationParameters.hpp"


struct RendererRequest {
    std::shared_ptr<IDrawable> drawable;
};

struct RendererState {
    std::deque<RendererRequest> requests;
    std::mutex queue_mutex;
    
    // TODO: probably it's better to make bools
    // non atomic and modify them under a lock
    std::atomic<bool> frame_active{false};
    std::atomic<bool> alive{true};
    std::mutex bool_mutex;
    
    std::condition_variable rendering_finished;
    std::condition_variable rendering_started;

    DrawInfo draw_info;
    const ApplicationParameters& params;
};

class RenderWorker {
public:
    RenderWorker(RendererState& state) : _state(state) {
        _logger = spdlog::stdout_color_st("render worker");
        _logger->set_level(state.params.log_level);
        _logger->info("Rendeer worker is created!");
    }

    void Start() {
        _logger->debug("starting render worker...");
        while (_state.alive.load()) {
            _logger->debug("awaiting new frame...");
            AwaitFrameStart();
            _logger->debug("processing render requests...");
            ProcessRequests();
            _logger->debug("finished rendering!");
            _state.rendering_finished.notify_all();
        }
        _logger->debug("stopping render worker");
    }

private:
    RendererState& _state;
    std::shared_ptr<spdlog::logger> _logger;

    void AwaitFrameStart() {
        std::unique_lock lock(_state.bool_mutex);
        _state.rendering_started.wait(lock, [this]{ return _state.frame_active.load(); });
    }

    bool AwaitRequest() {
        std::unique_lock lock(_state.queue_mutex);
        _state.rendering_started.wait(lock, [this]{ return !_state.requests.empty() || !_state.frame_active.load(); });
        return !_state.requests.empty();
    }

    void ProcessRequest(RendererRequest& request) {
        request.drawable->draw(_state.draw_info);
    }

    void ProcessRequests() {
        while(AwaitRequest()) {
            _state.queue_mutex.lock();
            RendererRequest request = _state.requests.front();
            _state.requests.pop_front();
            _state.queue_mutex.unlock();
            ProcessRequest(request);
        }
    }
};

class Renderer {
public:
    Renderer(const ApplicationParameters& params)
    :
    _state{.frame_active=true, .alive=true, .params=params} {
        _worker = std::thread([this]{
            RenderWorker worker(_state);
            worker.Start();
        });
    }

    //! Thread-safe. Will add request to the queue
    void AddRequest(RendererRequest request) {
        std::lock_guard lock(_state.queue_mutex);
        _state.requests.push_back(request);
        _state.rendering_started.notify_one();
    }

    void FinishFrame() {
        _state.frame_active.store(false);
    }

    //! Thread-safe. Will block until all requests are satisfied
    void Await() {
        // TODO: implement something less stupid, i don't like this
        std::unique_lock lock(_state.queue_mutex);
        _state.rendering_finished.wait(lock, [this]{ return !_state.requests.empty(); });
    }

    void StartFrame(const DrawInfo& info) {
        _state.draw_info = info;
        _state.frame_active.store(true);
        _state.rendering_started.notify_one();
    }

private:
    RendererState _state;

    std::thread _worker;
};

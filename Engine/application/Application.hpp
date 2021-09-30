#pragma once

#include "application/renderer/Renderer.hpp"
#include "application/ApplicationParameters.hpp"
#include "application/IScene.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <chrono>


class Application {
    using wall_clock_t = std::chrono::steady_clock;
public:
    Application(const ApplicationParameters& params, std::unique_ptr<IScene> initial_scene)
    : 
    _params(params),
    _renderer(params),
    _active_scene(std::move(initial_scene))
    {
        initLogger();
    }

    virtual ~Application() = default;

    void start() {
        _logger->info("started");
        _program_start = _wall_clock.now();

        prepareScene();
        _logger->debug("scene prepared");
        prepareInput();
        run();
    }

    InputManager inputManager;
private:
    ApplicationParameters _params;
    Renderer _renderer;
    std::shared_ptr<spdlog::logger> _logger;
    wall_clock_t _wall_clock;
    wall_clock_t::time_point _program_start;
    bool _alive = true;
    double _last_frame_time;
    std::unique_ptr<IScene> _active_scene;
    

    float seconds_since_start() {
        auto duration = _wall_clock.now() - _program_start;
        return static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) / 1000.0f;
    }

    void initLogger() {
        _logger = spdlog::stdout_color_mt("application");
        _logger->set_level(_params.log_level);
        if (!_params.log_location.empty()) {
            auto rotating = std::make_shared<spdlog::sinks::rotating_file_sink_mt> (
                _params.log_location, 1024*1024, 5, true
            );
            _logger->sinks().push_back(rotating);
        }
    }

    virtual void prepareScene() { 
        _active_scene->prepare();

        for (auto& obj : _active_scene->getElements()) {
            _renderer.AddRequest({RendererRequest::init, obj});
        }
    }

    void prepareInput() {
        while(!_renderer.IsInitialized()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        auto window = _renderer.getWindow();
        glfwMakeContextCurrent(window);

        inputManager.init(window);
    }

    void run() {
        _logger->info("Starting main loop");
        uint64_t frame = 0;
        float last_output = 0.0f;
        float last_fps_meter = seconds_since_start();
        float fps = 0.0f;
        uint64_t measure_every = 1000;

        while(_alive) {
            ProcessInput();
            ProcessFrame();
            if (frame % measure_every == 0) {
                auto since_start = seconds_since_start();
                if (since_start - last_output > 1.0f) {
                    _logger->info("Frame {}. Average fps: {}", frame, fps);
                    last_output = seconds_since_start();
                }
                fps = static_cast<float>(measure_every) / (since_start - last_fps_meter) * 0.9f + fps * 0.1f;
                last_fps_meter = since_start;
            }
            ++frame;
        }
        _logger->info("Closing the program");
    }

    float timeSinceLastFrame() {
        // maybe this should go to renderer
        double current_time = glfwGetTime();
        double dt = current_time - _last_frame_time;
        _last_frame_time = current_time;

        return static_cast<float>(dt);
    }

    DrawInfo generateDrawInfo() {
        auto cam_info = _active_scene->getCamera()->cameraInfo();
        DrawInfo renderInfo;
        renderInfo.view_mat = cam_info.viewMatrix;
        renderInfo.proj_mat = cam_info.projMatrix;
        renderInfo.seconds_since_start = seconds_since_start(); 
        return renderInfo;
    }

    UpdateInfo generateUpdateInfo() {
        return {
            .window = _renderer.getWindow(), 
            .dt = timeSinceLastFrame(),   
            .keyInputs = inputManager.extractKeyInputs(),
            .mouseMoveInputs = inputManager.extractMouseMoveEvents(),
            .mouseScrollInputs = inputManager.extractMouseScrollEvents()
        };
    }

    void ProcessInput() {
        _alive = inputManager.update();
    }

    void ProcessFrame() {
        auto draw_info = generateDrawInfo();
        auto update_info = generateUpdateInfo();

        _renderer.StartFrame(draw_info);

        // This could probably be parallelized
        for (auto& obj : _active_scene->getElements()) {
            obj->update(update_info);
            _renderer.AddRequest({ RendererRequest::draw, obj });
        }

        _renderer.FinishFrame();
        _renderer.Await();
    }
};

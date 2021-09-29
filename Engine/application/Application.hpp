#pragma once

#include "application/renderer/Renderer.hpp"
#include "application/ApplicationParameters.hpp"
#include "objects/primitives/Sphere.hpp"
#include "camera/Camera.hpp"
#include "objects/ISceneObject.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <chrono>


class Application {
    using wall_clock_t = std::chrono::steady_clock;
public:
    Application(const ApplicationParameters& params)
    : 
    _params(params),
    _renderer(params)
    {
        initLogger();
    }

    virtual ~Application() = default;

    void start() {
        _logger->info("started");
        _program_start = _wall_clock.now();
        //initGUI();

        //_logger->debug("GUI initialized");

        prepareScene();
        _logger->debug("scene prepared");
        prepareInput();
        run();
    }

private:
    ApplicationParameters _params;
    Renderer _renderer;
    std::shared_ptr<spdlog::logger> _logger;
    wall_clock_t _wall_clock;
    wall_clock_t::time_point _program_start;
    std::vector<std::shared_ptr<ISceneObject>> _objects;
    std::shared_ptr<FreeCameraMover> _cameraMover;

    float seconds_since_start() {
        auto duration = _wall_clock.now() - _program_start;
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / 1000.0f;
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
        _objects.push_back(std::make_shared<Sphere>(1.0f, 10));
        _objects.push_back(std::make_shared<Sphere>(1.0f, 1000));

        _cameraMover = std::make_unique<FreeCameraMover>(20.0f);
        _cameraMover.get()->setNearFarPlanes(_params.near_plane, _params.far_plane);
        _objects.push_back(_cameraMover);

        for (auto& obj : _objects) {
            _renderer.AddRequest({RendererRequest::init, obj});
        }
    }

    void prepareInput() {
        while(!_renderer.IsInitialized()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        glfwMakeContextCurrent(_renderer.getWindow());
    }

    void run() {
        _logger->info("Starting main loop");
        while(!glfwWindowShouldClose(_renderer.getWindow())) {
            glfwPollEvents();
            ProcessFrame();            
        }
        _logger->info("Closing the program");
    }

    float timeSinceLastFrame() {
        return 0.01f;
    }

    DrawInfo generateDrawInfo() {
        auto cam_info = _cameraMover->cameraInfo();
        DrawInfo renderInfo;
        renderInfo.view_mat = cam_info.viewMatrix;
        renderInfo.proj_mat = cam_info.projMatrix;
        renderInfo.seconds_since_start = seconds_since_start(); 
        return renderInfo;
    }

    UpdateInfo generateUpdateInfo() {
        return {
            .window=_renderer.getWindow(), 
            .dt = timeSinceLastFrame(),    
        };
    }

    void ProcessFrame() {
        auto draw_info = generateDrawInfo();
        auto update_info = generateUpdateInfo();

        _renderer.StartFrame(draw_info);

        // This could probably be parallelized
        for (auto& obj : _objects) {
            obj->update(update_info);
            _renderer.AddRequest({ RendererRequest::draw, obj });
        }

        _renderer.FinishFrame();
        _renderer.Await();
    }
};

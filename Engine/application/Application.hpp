#pragma once

#include "application/renderer/Renderer.hpp"
#include "application/ApplicationParameters.hpp"
#include "objects/primitives/Planet.hpp"
#include "camera/Camera.hpp"
#include "objects/ISceneObject.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <chrono>
#include "render/util/TextureContainer.hpp"


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
    std::vector<std::shared_ptr<ISceneObject>> _objects;
    std::shared_ptr<FreeCameraMover> _cameraMover;
    bool _alive = true;
    wall_clock_t::time_point _last_frame_time;
    

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
        _cameraMover = std::make_shared<FreeCameraMover>(20.0f);
        _cameraMover.get()->setNearFarPlanes(_params.near_plane, _params.far_plane);
        _objects.push_back(_cameraMover);

        _objects.push_back(std::make_shared<Planet>("sun", 3.39f * 16.0f, 0.0f, 1.0f, 25.3f, "2k_sun.jpg"));
        _objects.push_back(std::make_shared<Planet>("mercury", 2.4f, 77.9f, 87.9f, 58.6, "2k_mercury.jpg"));
        _objects.push_back(std::make_shared<Planet>("venus", 6.05f, 128.2f, 224.7f, 243.0f,  "2k_venus_surface.jpg"));
        _objects.push_back(std::make_shared<Planet>("earth", 6.37f, 169.5f, 365.2f, 0.99f, "2k_earth_daymap.jpg"));
        _objects.push_back(std::make_shared<Planet>("mars", 3.39f, 247.9f, 686.9f, 1.1f, "2k_mars.jpg"));
        _objects.push_back(std::make_shared<Planet>("jupiter", 55.49f, 798.5f, 4332.5f, 9.9f/24.0f, "2k_jupiter.jpg"));
        _objects.push_back(std::make_shared<Planet>("saturn", 48.26f,  1449.3f, 10759.22f, 10.5f / 24.0f, "2k_saturn.jpg"));
        _objects.push_back(std::make_shared<Planet>("uranus", 22.55f, 2896.67f, 30685.4, 17.2f / 24.0f, "2k_uranus.jpg"));
        _objects.push_back(std::make_shared<Planet>("neptune", 20.76f, 4523.44f, 60190.03f, 0.66f, "2k_neptune.jpg"));

        for (auto& obj : _objects) {
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
        return 0.0001f;
        // auto now = _wall_clock.now();
        // auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(now - _last_frame_time);
        // _last_frame_time = now;
        // return static_cast<float>(nanos.count()) / 1000000000.0f;
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
        for (auto& obj : _objects) {
            obj->update(update_info);
            _renderer.AddRequest({ RendererRequest::draw, obj });
        }

        _renderer.FinishFrame();
        _renderer.Await();
    }
};

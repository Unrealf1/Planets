#pragma once

#include "application/Renderer.hpp"
#include "application/ApplicationParameters.hpp"
#include "objects/primitives/Sphere.hpp"
#include "camera/Camera.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include <imgui.h>
#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


class Application {
public:
    Application(const ApplicationParameters& params)
    : 
    _params(params),
    _renderer(params)
    {
        initLogger();
    }

    void start() {
        _logger->info("started");
        initContext();
        _logger->debug("context initialized");
        initGL();
        _logger->debug("GL initialized");
        initGUI();

        _logger->debug("GUI initialized");

        prepareScene();
        _logger->debug("scene prepared");
        run();
    }

private:
    ApplicationParameters _params;
    Renderer _renderer;
    std::shared_ptr<spdlog::logger> _logger;
    GLFWwindow* _window = nullptr;

    void initLogger() {
        _logger = spdlog::stdout_color_mt("application");
        _logger->set_level(_params.log_level);
        spdlog::set_level(_params.log_level);
        if (!_params.log_location.empty()) {
            auto rotating = std::make_shared<spdlog::sinks::rotating_file_sink_mt> (
                _params.log_location, 1024*1024, 5, true
            );
            _logger->sinks().push_back(rotating);
        }
    }

    void initContext() {
        //Инициализируем библиотеку GLFW
        if (!glfwInit()) {
            _logger->error("could not start GLFW3");
            exit(1);
        }

        //Устанавливаем параметры создания графического контекста
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        //Создаем графический контекст (окно)
        GLFWwindow* window = glfwCreateWindow(800, 600, _params.window_name.c_str(), nullptr, nullptr);
        if (!window) {
            _logger->error("could not open window with GLFW3");
            glfwTerminate();
            exit(1);
        }
        //Делаем этот контекст текущим
        glfwMakeContextCurrent(window);
        _window = window;
        _logger->debug("created window");

        glfwSwapInterval(0); //Отключаем вертикальную синхронизацию
        glfwSetWindowUserPointer(_window, this); //Регистрируем указатель на данный объект, чтобы потом использовать его в функциях обратного вызова}

        //glfwSetKeyCallback(_window, keyCallback); //Регистрирует функцию обратного вызова для обработки событий клавиатуры
        //glfwSetWindowSizeCallback(_window, windowSizeChangedCallback);
        //glfwSetMouseButtonCallback(_window, mouseButtonPressedCallback);
        //glfwSetCursorPosCallback(_window, mouseCursosPosCallback);
        //glfwSetScrollCallback(_window, scrollCallback);
        //glfwSetErrorCallback(glfw_error_callback);
    }

    void initGL() {
        glewExperimental = GL_TRUE;
        glewInit();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }  

    virtual void initGUI() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui_ImplGlfw_InitForOpenGL(_window, false);
        ImGui_ImplOpenGL3_Init("#version 430");
    }

    std::shared_ptr<Sphere> sphere;
    virtual void prepareScene() { 
        sphere = std::make_shared<Sphere>(1.0f, 100);
        sphere->init();
    }

    void run() {
        auto _cameraMover = std::make_unique<FreeCameraMover>(20.0f);
        _cameraMover.get()->setNearFarPlanes(_params.near_plane, _params.far_plane);



        while(true) {
            int width, height;
            glfwGetFramebufferSize(_window, &width, &height);

            glViewport(0, 0, width, height);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            auto cam_info = _cameraMover->cameraInfo();
            DrawInfo renderInfo;
            renderInfo.view_mat = cam_info.viewMatrix;
            renderInfo.proj_mat = cam_info.projMatrix;

            _renderer.StartFrame(renderInfo);
            _renderer.AddRequest({sphere});
            _renderer.FinishFrame();
            _renderer.Await();
        }
        
    }
};

#include "application/renderer/RenderWorker.hpp"


void RenderWorker::Init() {
    std::lock_guard lock(_state.state_mutex);
    InitWindow();
    if (_state.initialized) {
        return;
    }
    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    _state.initialized = true;

    _logger->debug("Opengl Initialized");
}

void RenderWorker::InitWindow() {
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
    GLFWwindow* window = glfwCreateWindow(800, 600, _state.params.window_name.c_str(), nullptr, nullptr);
    if (!window) {
        _logger->critical("could not open window with GLFW3");
        glfwTerminate();
        exit(1);
    }
    //Делаем этот контекст текущим
    glfwMakeContextCurrent(window);
    _state.window = window;
    _logger->debug("created window");

    glfwSwapInterval(0); //Отключаем вертикальную синхронизацию
    
    //glfwSetWindowUserPointer(_window, this); //Регистрируем указатель на данный объект, чтобы потом использовать его в функциях обратного вызова}
    //glfwSetKeyCallback(_window, keyCallback); //Регистрирует функцию обратного вызова для обработки событий клавиатуры
    //glfwSetWindowSizeCallback(_window, windowSizeChangedCallback);
    //glfwSetMouseButtonCallback(_window, mouseButtonPressedCallback);
    //glfwSetCursorPosCallback(_window, mouseCursosPosCallback);
    //glfwSetScrollCallback(_window, scrollCallback);
    //glfwSetErrorCallback(glfw_error_callback);

    _logger->debug("Window is created");
}


/**
 * Returns true, if there're requests ready for consumption
 * Returns false if there won't be new requests and frame is finished
 */
bool RenderWorker::AwaitRequest() {
    _logger->debug("que size: {}, current draw frame: {}, active frame: {}", _state.requests.size(), _state.draw_frame, _state.active_frame);
    std::unique_lock lock(_state.state_mutex);
    _state.rendering_started.wait(lock, [this]{ 
        return !_state.requests.empty() || (_state.active_frame > _state.draw_frame) || !_state.alive; 
    });
    //_logger->debug("que size: {}, current draw frame: {}, active frame: {}", _state.requests.size(), _state.draw_frame, _state.active_frame);
    return (!_state.requests.empty()) & (_state.alive);
}

/**
 * Waits until state.draw_info is ready
 */ 
void RenderWorker::AwaitFrameStart() {
    std::unique_lock lock(_state.state_mutex);
    _state.rendering_started.wait(lock, [this] { return _state.frame_ready || (_state.active_frame > _state.draw_frame) || !_state.alive; });
}

void RenderWorker::ProcessRequest(RendererRequest& request) {
    _logger->debug("processing request of type {}", request.type);
    switch (request.type) {
    case RendererRequest::draw:
        request.drawable->draw(_state.draw_info);
        break;
    case RendererRequest::init:
        request.drawable->init();
        break;
    }
}

void RenderWorker::ProcessRequests() {
    while(AwaitRequest()) {
        _state.state_mutex.lock();
        RendererRequest request = _state.requests.front();
        _state.requests.pop_front();
        _state.state_mutex.unlock();
        ProcessRequest(request);
    }
}

void RenderWorker::ResetFrame() {
    glfwSwapBuffers(_state.window);
    int width, height;
    glfwGetFramebufferSize(_state.window, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderWorker::Start() {
    _logger->debug("starting render worker...");
    Init();
    while (_state.alive) {
        //AwaitFrameStart();
        ResetFrame();
        _logger->error("processing render requests...");
        ProcessRequests();
        _logger->error("finished rendering!");
        // considering next frame as current draw frame 
        
        _state.state_mutex.lock();
        ++_state.draw_frame;
        _state.state_mutex.unlock();

        _state.rendering_finished.notify_all();
    }
    _logger->error("stopping render worker");
}

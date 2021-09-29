#pragma once

#include "input/InputEvent.hpp"
#include <vector>
#include <GLFW/glfw3.h>
#include <mutex>


static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouseCursosPosCallback(GLFWwindow* window, double xpos, double ypos);
static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

class InputManager {
public:
    using TKeyInputs = std::vector<KeyInputEvent>;
    using TMouseMoveInputs = std::vector<MouseMoveInputEvent>;
    using TMouseScrollInputs = std::vector<MouseScrollInputEvent>;

public:
    void init(GLFWwindow* window) {
        _window = window;
        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, keyCallback);
        glfwSetCursorPosCallback(window, mouseCursosPosCallback);
        glfwSetScrollCallback(window, scrollCallback);
    }

    bool update() {
        glfwPollEvents();
        return !glfwWindowShouldClose(_window);
    }

    TKeyInputs extractKeyInputs() {
        std::lock_guard lock(_containers_lock);
        auto copy = _keyInputs;
        _keyInputs.clear();
        return copy;
    }

    TMouseMoveInputs extractMouseMoveEvents() {
        std::lock_guard lock(_containers_lock);
        auto copy = _mouseMoveInputs;
        _mouseMoveInputs.clear();
        return copy;
    }

    TMouseScrollInputs extractMouseScrollEvents() {
        std::lock_guard lock(_containers_lock);
        auto copy = _mouseScrollInputs;
        _mouseScrollInputs.clear();
        return copy;
    }

    void handleKeyEvent(int key, int scancode, int action, int mods) {
        std::lock_guard lock(_containers_lock);
        _keyInputs.push_back( { key, scancode, action, mods } );
    }

    void handleMouseMove(double xpos, double ypos) {
        std::lock_guard lock(_containers_lock);
        auto left_state = glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT);
        auto right_state = glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_RIGHT);
        auto middle_state = glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_MIDDLE);
        _mouseMoveInputs.push_back( { xpos, ypos, { left_state, right_state, middle_state } } );
    }

    void handleMouseScroll(double xoffset, double yoffset) {
        std::lock_guard lock(_containers_lock);
        auto left_state = glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT);
        auto right_state = glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_RIGHT);
        auto middle_state = glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_MIDDLE);
        _mouseScrollInputs.push_back( { xoffset, yoffset, { left_state, right_state, middle_state } } );
    }

private:
    TKeyInputs _keyInputs;
    TMouseMoveInputs _mouseMoveInputs;
    TMouseScrollInputs _mouseScrollInputs;
    // TODO: 
    // this is probably unnecessary. should think about this later
    std::mutex _containers_lock;

    GLFWwindow* _window;
};

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto manager = reinterpret_cast<InputManager*>(glfwGetWindowUserPointer(window));
    manager->handleKeyEvent(key, scancode, action, mods);
}

static void mouseCursosPosCallback(GLFWwindow* window, double xpos, double ypos) {
    auto manager = reinterpret_cast<InputManager*>(glfwGetWindowUserPointer(window));
    manager->handleMouseMove(xpos, ypos);
}

static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto manager = reinterpret_cast<InputManager*>(glfwGetWindowUserPointer(window));
    manager->handleMouseScroll(xoffset, yoffset);
}

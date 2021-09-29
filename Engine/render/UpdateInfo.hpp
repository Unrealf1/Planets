#pragma once

#include <GLFW/glfw3.h>
#include <memory>
#include "input/InputManager.hpp"


struct UpdateInfo {
    GLFWwindow* window;
    double dt; //!< Time since last frame
    InputManager::TKeyInputs keyInputs;
    InputManager::TMouseMoveInputs mouseMoveInputs;
    InputManager::TMouseScrollInputs mouseScrollInputs;
};

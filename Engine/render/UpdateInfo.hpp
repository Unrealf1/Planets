#pragma once

#include <GLFW/glfw3.h>
#include <memory>


struct UpdateInfo {
    GLFWwindow* window;
    double dt; //!< Time since last frame
};

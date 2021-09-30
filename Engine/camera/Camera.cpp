/**
 * Code here is mostly taken from 
 * https://github.com/AlexBobkov/graphicssamples/tree/4c00e1dc4d61c7d97ed52c9dd7b78b32bd219c12
 */

#include "camera/Camera.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <imgui.h>

#include <iostream>

void OrbitCameraMover::handleKey(const KeyInputEvent&) { }

void OrbitCameraMover::showOrientationParametersImgui() {
    ImGui::Text("r = %.2f, phi = %.2f, theta = %2f", _r, _phiAng, _thetaAng);
}

void OrbitCameraMover::handleMouseMove(const MouseMoveInputEvent& event) {
    auto state = event.mouse.left_button;
    auto xpos = event.xpos;
    auto ypos = event.ypos;

    if (state == GLFW_PRESS)
    {
        double dx = xpos - _oldXPos;
        double dy = ypos - _oldYPos;

        _phiAng -= dx * 0.005;
        _thetaAng += dy * 0.005;

        _thetaAng = glm::clamp(_thetaAng, -glm::pi<double>() * 0.49, glm::pi<double>() * 0.49);
    }

    _oldXPos = xpos;
    _oldYPos = ypos;
}

void OrbitCameraMover::handleScroll(const MouseScrollInputEvent& event) {
    _r += _r * event.yoffset * 0.05;
}

void OrbitCameraMover::update(const UpdateInfo& info) {
    for (auto& event : info.mouseMoveInputs) {
        handleMouseMove(event);
    }

    for (auto& event : info.mouseScrollInputs) {
        handleScroll(event);
    }

    GLFWwindow* window = info.window;
    double dt = info.dt;
    double speed = 1.0;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        _phiAng -= speed * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        _phiAng += speed * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        _thetaAng += speed * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        _thetaAng -= speed * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        _r += _r * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        _r -= _r * dt;
    }

    _thetaAng = glm::clamp(_thetaAng, -glm::pi<double>() * 0.49, glm::pi<double>() * 0.49);

    //-----------------------------------------

    //Вычисляем положение виртуальной камеры в мировой системе координат по формуле сферических координат
    glm::vec3 pos = glm::vec3(
        glm::cos(_phiAng) * glm::cos(_thetaAng), 
        glm::sin(_phiAng) * glm::cos(_thetaAng), 
        glm::sin(_thetaAng) + 0.5
    ) * static_cast<float>(_r);

    //Обновляем матрицу вида
    _camera.viewMatrix = glm::lookAt(pos, glm::vec3(0.0f, 0.0f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f));

    //-----------------------------------------

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    //Обновляем матрицу проекции на случай, если размеры окна изменились
    _camera.projMatrix = glm::perspective(
        glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), _near, _far
    );
}

//=============================================

FreeCameraMover::FreeCameraMover(float speed = 5.0f) :
CameraMover(),
speed(speed),
_pos(5.0f, 0.0f, 2.5f)
{       
    //Нам нужно как-нибудь посчитать начальную ориентацию камеры
    _rot = glm::toQuat(glm::lookAt(_pos, glm::vec3(0.0f, 0.0f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f)));
}

void FreeCameraMover::handleKey(const KeyInputEvent&) { }

void FreeCameraMover::handleMouseMove(const MouseMoveInputEvent& event)  {
    //int left_button_state, double xpos, double ypos
    auto state = event.mouse.left_button;
    auto xpos = event.xpos;
    auto ypos = event.ypos;
    if (state == GLFW_PRESS) {
        auto dx = xpos - _oldXPos;
        auto dy = ypos - _oldYPos;

        //Добавляем небольшой поворот вверх/вниз        
        glm::vec3 rightDir = glm::vec3(1.0f, 0.0f, 0.0f) * _rot;
        _rot *= glm::angleAxis(static_cast<float>(dy * 0.005), rightDir);

        //Добавляем небольшой поворов вокруг вертикальной оси
        glm::vec3 upDir(0.0f, 0.0f, 1.0f);
        _rot *= glm::angleAxis(static_cast<float>(dx * 0.005), upDir);
    }

    _oldXPos = xpos;
    _oldYPos = ypos;
}

void FreeCameraMover::handleScroll(const MouseScrollInputEvent&) { }

void FreeCameraMover::update(const UpdateInfo& info) {
    for (auto& event : info.mouseMoveInputs) {
        handleMouseMove(event);
    }

    GLFWwindow* window = info.window;
    double dt = info.dt;

    float actual_speed = speed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        actual_speed *= 2;
    }
    //Получаем текущее направление "вперед" в мировой системе координат
    glm::vec3 forwDir = glm::vec3(0.0f, 0.0f, -1.0f) * _rot;

    //Получаем текущее направление "вправо" в мировой системе координат
    glm::vec3 rightDir = glm::vec3(1.0f, 0.0f, 0.0f) * _rot;

    glm::vec3 upDir = glm::vec3(0.0f, 1.0f, 0.0f) * _rot;
    
    //Двигаем камеру вперед/назад
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        _pos += forwDir * actual_speed * static_cast<float>(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        _pos -= forwDir * actual_speed * static_cast<float>(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        _pos -= rightDir * actual_speed * static_cast<float>(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        _pos += rightDir * actual_speed * static_cast<float>(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
        _pos -= upDir * actual_speed * static_cast<float>(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        _pos += upDir * actual_speed * static_cast<float>(dt);
    }

    //-----------------------------------------

    //Соединяем перемещение и поворот вместе
    _camera.viewMatrix = glm::toMat4(-_rot) * glm::translate(-_pos);
    
    //-----------------------------------------

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    //Обновляем матрицу проекции на случай, если размеры окна изменились
    _camera.projMatrix = glm::perspective(
        glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), _near, _far
    );
}

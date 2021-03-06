/**
 * Code here is mostly taken from 
 * https://github.com/AlexBobkov/graphicssamples/blob/4c00e1dc4d61c7d97ed52c9dd7b78b32bd219c12/common/Camera.hpp 
 */

#pragma once

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "objects/ISceneObject.hpp"
#include "input/InputEvent.hpp"

#include <memory>

struct CameraInfo
{
    glm::mat4 viewMatrix;
    glm::mat4 projMatrix;
};

/**
Класс для управления виртуальной камерой
*/
class CameraMover : public ISceneObject {
public:
    CameraMover() {}
    virtual ~CameraMover() {}

    bool isMarkedForDeletion() { return false; }
    void markForDeletion() { }
    void draw(const DrawInfo&) override { }
    void init() override { }

    /**
    Возвращает параметры виртуальной камеры в виде 2х матриц: матрицы вида и проекции
    */
    CameraInfo cameraInfo() const { return _camera; }

    /**
    Обрабатывает нажатия кнопок на клавитуре.
    См. сигнатуру GLFWkeyfun библиотеки GLFW
    */
    virtual void handleKey(const KeyInputEvent&) = 0;

    /**
    Обрабатывает движение мышки
    */
    virtual void handleMouseMove(const MouseMoveInputEvent&) = 0;

    /**
    Обрабатывает колесико мыши
    */
    virtual void handleScroll(const MouseScrollInputEvent&) = 0;

    /**
    Обновляет положение виртуальной камеры
    */
    virtual void update(const UpdateInfo& info) = 0;

    virtual void setNearFarPlanes(float near, float far) { _near = near; _far = far; }

	virtual void showOrientationParametersImgui() { }
    virtual glm::vec3 getPos() = 0;
    // virtual void setSpeed(float) = 0;
    // virtual float getSpeed() = 0;
protected:
    CameraInfo _camera;
    float _near = 0.1f;
    float _far = 100.0f;
};

using CameraMoverPtr = std::shared_ptr<CameraMover>;

/**
Положение камеры задается в сферических координатах. Камера всегда смотрит в центр сферы.
*/
class OrbitCameraMover : public CameraMover
{
public:
    void handleKey(const KeyInputEvent&) override;
    void handleMouseMove(const MouseMoveInputEvent&) override;
    void handleScroll(const MouseScrollInputEvent&) override;
    void update(const UpdateInfo& info) override;

	void showOrientationParametersImgui() override;
	void setOrientationParameters(double r, double phi, double theta) { _r = r; _phiAng = phi; _thetaAng = theta; }
    glm::vec3 getPos() override {
        return glm::vec3(
            glm::cos(_phiAng) * glm::cos(_thetaAng), 
            glm::sin(_phiAng) * glm::cos(_thetaAng), 
            (glm::sin(_thetaAng) + 0.5)
        ) * static_cast<float>(_r);
    }
protected:
    //Положение виртуальный камеры задается в сферических координат
    double _phiAng = 0.0;
    double _thetaAng = 0.0;
    double _r = 5.0;

    //Положение курсора мыши на предыдущем кадре
    double _oldXPos = 0.0;
    double _oldYPos = 0.0;
};

/**
Камера всегда летит вперед
*/
class FreeCameraMover : public CameraMover
{
public:
    FreeCameraMover(
        float speed, 
        glm::vec3 starting_position, 
        const glm::vec3& looking_at, 
        const glm::vec3& up = {0.0f, 0.0f, 1.0f}
    );

    void handleKey(const KeyInputEvent&) override;
    void handleMouseMove(const MouseMoveInputEvent&) override;
    void handleScroll(const MouseScrollInputEvent&) override;
    void update(const UpdateInfo& info) override;
    glm::vec3 getPos() override {
        return _pos;
    }
    void setSpeed(float spd) {
        speed = spd;
    }

    float getSpeed() {
        return speed;
    }
protected:
    float speed;
    glm::vec3 _pos;
    glm::quat _rot;

    //Положение курсора мыши на предыдущем кадре
    double _oldXPos = 0.0;
    double _oldYPos = 0.0;
};


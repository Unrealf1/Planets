#pragma once

#include "application/IScene.hpp"
#include "application/ApplicationParameters.hpp"
#include "game_objects/Planet.hpp"
#include "camera/Camera.hpp"
#include "game_objects/GameGui.hpp"


class GameScene : public IBasicScene {
public:
    GameScene(const ApplicationParameters& params)
    :
    _near_plane(params.near_plane),
    _far_plane(params.far_plane) { }

    void prepare(GLFWwindow* window) override {
        _cameraMover = std::make_shared<FreeCameraMover>(20.0f, glm::vec3{85.0f, 0.0f, 20.0f}, glm::vec3{85.0f, 1.0f, 20.0f});
        _cameraMover.get()->setNearFarPlanes(_near_plane, _far_plane);
        addElement(_cameraMover);

        addElement(std::make_shared<Planet>("sun", 3.39f * 16.0f, 0.0f, 1.0f, 25.3f, "2k_sun.jpg"));
        addElement(std::make_shared<Planet>("mercury", 2.4f, 77.9f, 87.9f, 58.6, "2k_mercury.jpg"));
        addElement(std::make_shared<Planet>("venus", 6.05f, 128.2f, 224.7f, 243.0f,  "2k_venus_surface.jpg"));
        addElement(std::make_shared<Planet>("earth", 6.37f, 169.5f, 365.2f, 0.99f, "2k_earth_daymap.jpg"));
        addElement(std::make_shared<Planet>("mars", 3.39f, 247.9f, 686.9f, 1.1f, "2k_mars.jpg"));
        addElement(std::make_shared<Planet>("jupiter", 55.49f, 798.5f, 4332.5f, 9.9f/24.0f, "2k_jupiter.jpg"));
        addElement(std::make_shared<Planet>("saturn", 48.26f,  1449.3f, 10759.22f, 10.5f / 24.0f, "2k_saturn.jpg"));
        addElement(std::make_shared<Planet>("uranus", 22.55f, 2896.67f, 30685.4, 17.2f / 24.0f, "2k_uranus.jpg"));
        addElement(std::make_shared<Planet>("neptune", 20.76f, 4523.44f, 60190.03f, 0.66f, "2k_neptune.jpg"));

        //Imgui is demanding. It did cost me about 5k fps :)
        addElement(std::make_shared<GameGui>(window));
    }

    std::shared_ptr<CameraMover> getCamera() {
        return _cameraMover;
    }

private:
    std::shared_ptr<FreeCameraMover> _cameraMover;
    float _near_plane;
    float _far_plane;
};

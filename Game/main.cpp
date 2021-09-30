#include <iostream>

#include "application/ApplicationParameters.hpp"
#include "application/Application.hpp"
#include "GameScene.hpp"


int main() {
    ApplicationParameters params = ApplicationParametersReader::read("parameters.json");
    std::unique_ptr<IScene> scene = std::make_unique<GameScene>(params);
    Application app(params, std::move(scene));
    app.start();
}
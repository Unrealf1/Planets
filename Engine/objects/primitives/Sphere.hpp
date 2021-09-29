#pragma once

#include "objects/ISceneObject.hpp"
#include "objects/Defaults.hpp"
#include "render/util/GraphicsInitializer.hpp"
#include "shader/ProgramContainer.hpp"
#include "shader/Program.hpp"

#include <cmath>
#include <array>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>


class Sphere : public BasicSceneObject {
public:
    Sphere(float radius, uint64_t num_polygons) 
    :
    _num_polygons(num_polygons),
    _position(default_position), 
    _rotation(default_rotation), 
    _scale(radius) {

    }

    void draw(const DrawInfo& info);

    void init() override {
        _model = createSphereModel(_num_polygons);
        std::cout << "model size is " << _model.mesh.size() << std::endl;
        _graphics = GraphicsInitializer::initObject(_model);

        if (ProgramContainer::checkProgram("simple prog")) {
            return;
        }

        spdlog::debug("Sphere: creating program ...");

        auto text = extractShaderText("resources/shaders/Basic.vert");
        auto vertex_shader = createVertexShader(text.c_str());

        text = extractShaderText("resources/shaders/Basic.frag");
        auto frag_shader = createFragmentShader(text.c_str());

        auto program = createProgram(vertex_shader, frag_shader);
        ProgramContainer::registerProgram("simple prog", program);
    }

    void update(const UpdateInfo&) { }

    glm::vec3 getPosition() const {
        return _position;
    }

    glm::quat getRotation() const {
        return _rotation;
    }

    glm::vec3 getScale() const {
        return _scale;
    }

    //! Will create model with _at least_ @param num_polygons polygons
    static Model<> createSphereModel(uint64_t num_polygons) {
        // Code was written looking at [this](http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html) article
        spdlog::debug("Sphere: initializing...");
        auto current_sphere = getIcosahedronTriangles();
        while (current_sphere.size() < num_polygons) {
            current_sphere = refineSphere(current_sphere);
        }
        auto mesh = FlattenTriangles(current_sphere);
        spdlog::debug("Sphere: model created!");
        return {mesh, mesh};
    }

private:
    uint64_t _num_polygons;
    glm::vec3 _position;
    glm::quat _rotation;
    glm::vec3 _scale;
    Model<> _model;

    GraphicObject _graphics;

    static std::vector<Triangle<>> getIcosahedronTriangles();
    static std::vector<Triangle<>> refineSphere(const std::vector<Triangle<>>& current_sphere);
};

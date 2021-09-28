#pragma once

#include "objects/ISceneObject.hpp"
#include "objects/Defaults.hpp"
#include "render/util/GraphicsInitializer.hpp"

#include <cmath>
#include <array>


class Sphere : public BasicSceneObject {
public:
    Sphere(float radius, uint64_t num_polygons) 
    : 
    _position(default_position), 
    _rotation(default_rotation), 
    _scale(radius) {

    }

    void init() override {

    }

    //! Will create model with _at least_ @param num_polygons polygons
    static Model createSphereModel(uint64_t num_polygons) {
        // Code was written looking at [this](http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html) article

        auto current_sphere = getIcosahedronTriangles();
        while (current_sphere.size() < num_polygons) {
            current_sphere = refineSphere(current_sphere);
        }
    }

private:
    glm::vec3 _position;
    glm::quat _rotation;
    glm::vec3 _scale;
    Model _model;

    GraphicObject _graphics;

    static std::vector<Triangle> getIcosahedronTriangles();
    static std::vector<Triangle> refineSphere(const std::vector<Triangle>& current_sphere) {
        
    }
};

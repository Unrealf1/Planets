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


class Sphere {
public:
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
    static std::vector<Triangle<>> getIcosahedronTriangles();
    static std::vector<Triangle<>> refineSphere(const std::vector<Triangle<>>& current_sphere);
};

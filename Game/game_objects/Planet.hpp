#pragma once

#include "objects/primitives/Sphere.hpp"
#include "objects/primitives/DrawableModel.hpp"
#include "render/util/TextureContainer.hpp"

#include <string>
#include <cmath>
#include <numbers>
#include <algorithm>


class Planet : public ISceneObject {
public:
    Planet(
        std::string name, 
        float self_radius, 
        float system_radius, 
        float full_cycle_time,
        float self_rotation_time,
        std::string texture_name
    )
    :
    _name(std::move(name)),
    _self_radius(self_radius),
    _system_radius(system_radius),
    _full_cycle_time(full_cycle_time),
    _self_rotation_time(self_rotation_time) {
        auto model = Sphere::createSphereModel(static_cast<uint64_t>(200.0f / 6.0f * self_radius));
        
        auto get_uv = [](glm::vec3 norm_sphere_coords) -> glm::vec2 {
            // TODO: fix uv-mapping. maybe use https://en.wikipedia.org/wiki/Mercator_projection
            auto x = (norm_sphere_coords.x + 1.0f) / 2.0f;
            auto y = (norm_sphere_coords.y + 1.0f) / 2.0f;
            auto z = (norm_sphere_coords.z + 1.0f) / 2.0f;
            float theta = std::atan(std::sqrt(x*x + y*y) / z);
            float phi = std::atan(y / x);
            return glm::vec2{theta, phi} / std::numbers::pi_v<float>;
        };

        Mesh<glm::vec2> tex_coords;
        tex_coords.reserve(model.mesh.size());

        std::ranges::transform(model.mesh, std::back_inserter(tex_coords), get_uv);

        _graphics = std::make_unique<DrawableModel>(
            TexturedModel<>{std::move(model), std::move(tex_coords)}, 
            TextureContainer::getFullTexturePath(texture_name)
        );
    }

    void init() override {
        _graphics->init();
        _graphics->setScale(glm::vec3(_self_radius));
    }

    void draw(const DrawInfo& info) {
        _graphics->draw(info);
    }

    void update(const UpdateInfo& info) {
        _position.x = std::sin(_angle) * _system_radius;
        _position.z = 0.0f;
        _position.y = std::cos(_angle) * _system_radius;
        _angle += static_cast<float>(info.dt) * std::numbers::pi_v<float> * 2.0f / (_full_cycle_time);
        _self_rotation += static_cast<float>(info.dt) * std::numbers::pi_v<float> * 2.0f / (_self_rotation_time);

        _graphics->setRotation(glm::angleAxis(_self_rotation, glm::vec3(0.0f, 0.0f, 1.0f)));
        _graphics->setPosition(_position);
        _graphics->update(info);
    }

    bool isMarkedForDeletion() {
        return false;
    }

    void markForDeletion() { }

private:
    std::unique_ptr<DrawableModel> _graphics;
    std::string _name;
    float _self_radius;         //!< In 10^3 km
    float _system_radius;       //!< In 10^6 km
    float _full_cycle_time;     //!< In Earth days (sideris)
    float _self_rotation_time;  //!< In Earth days
    
    glm::vec3 _position;
    float _angle = 0.0f;
    float _self_rotation = 0.0f;
};


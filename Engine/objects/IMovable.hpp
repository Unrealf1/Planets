#pragma once

#include "render/RenderDefs.hpp"


class IMovable {
public:
    IMovable() = default;
    virtual ~IMovable() = default;

    virtual glm::vec3 getPosition() const = 0;
};

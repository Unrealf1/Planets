#pragma once

#include "render/RenderDefs.hpp"


class IRotatable {
public:
    IRotatable() = default;
    virtual ~IRotatable() = default;

    virtual glm::quat getRotation() const = 0;
    virtual void setRotation(const glm::quat&) = 0;
};

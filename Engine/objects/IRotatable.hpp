#pragma once

#include "render/RenderDefs.hpp"


class IRotatable {
public:
    IRotatable() = default;
    virtual ~IRotatable() = default;

    virtual glm::quat getRotation() const = 0;
};

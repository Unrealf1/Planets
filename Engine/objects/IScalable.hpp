#pragma once

#include "render/RenderDefs.hpp"


class IScalable {
public:
    IScalable() = default;
    virtual ~IScalable() = default;

    virtual glm::vec3 getScale() const = 0;
};

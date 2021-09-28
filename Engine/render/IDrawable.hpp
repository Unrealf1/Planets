#pragma once

#include "render/DrawInfo.hpp"
#include "render/RenderDefs.hpp"

class DrawableUnit {
public:
    DrawableUnit() = default;
    virtual ~DrawableUnit() = default;

    virtual void draw(const DrawInfo&) = 0;
    virtual void init() = 0;    
};

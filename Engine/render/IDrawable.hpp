#pragma once

#include "render/DrawInfo.hpp"
#include "render/RenderDefs.hpp"

class IDrawable {
public:
    IDrawable() = default;
    virtual ~IDrawable() = default;

    virtual void draw(const DrawInfo&) = 0;
    virtual void init() = 0;    
};

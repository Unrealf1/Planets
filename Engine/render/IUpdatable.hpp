#pragma once

#include "render/UpdateInfo.hpp"


class Updatable {
public:
    Updatable() = default;
    virtual ~Updatable() = default;

    virtual void update(const UpdateInfo&) = 0;
};
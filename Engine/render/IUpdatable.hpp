#pragma once

#include "render/UpdateInfo.hpp"


class IUpdatable {
public:
    IUpdatable() = default;
    virtual ~IUpdatable() = default;

    virtual void update(const UpdateInfo&) = 0;
};
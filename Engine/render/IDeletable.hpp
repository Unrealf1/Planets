#pragma once

class IDeletable {
public:
    IDeletable() = default;
    virtual ~IDeletable() = default;

    virtual bool isMarkedForDeletion() = 0;
    virtual void markForDeletion() = 0;
};

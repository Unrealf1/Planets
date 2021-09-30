#pragma once

#include "objects/ISceneObject.hpp"
#include "camera/Camera.hpp"

#include <vector>
#include <memory>


/**
 * Scene is a container for scene objects
 * scene is not in charge of initializing objects
 */
class IScene {
public:
    using elements_t = std::vector<std::shared_ptr<ISceneObject>>;

public:
    IScene() = default;
    virtual ~IScene() = default;
    virtual void prepare() = 0;
    virtual elements_t& getElements() = 0;
    virtual void addElement(std::shared_ptr<ISceneObject>) = 0;
    virtual std::shared_ptr<CameraMover> getCamera() = 0;
};

class IBasicScene : public IScene {
public:
    IBasicScene() = default;
    virtual ~IBasicScene() = default;
    
    elements_t& getElements() override {
        return _elements;
    }

    void addElement(std::shared_ptr<ISceneObject> obj) override {
        _elements.push_back(obj);
    }

protected:
    elements_t _elements;
};

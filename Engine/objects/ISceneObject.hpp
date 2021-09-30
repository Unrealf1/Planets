#pragma once

#include "render/IDrawable.hpp"
#include "render/IUpdatable.hpp"
#include "render/IDeletable.hpp"
#include "objects/IMovable.hpp"
#include "objects/IRotatable.hpp"
#include "objects/IScalable.hpp"

/**
 * All that is displayed on the screen should inherit from this interface.
 * 
 * Realizations of this interface have to provide this methods:
 * void draw(const DrawInfo&)
 * void init()
 * void update(const UpdateInfo&)
 * bool isMarkedForDeletion()
 * void markForDeletion()
 */
class ISceneObject : public IDrawable, public IUpdatable, public IDeletable { };

/**
 * Realizations of this interface have to provide all of requirements of `SceneObject` and additionally
 * glm::vec3 getPosition() const
 * glm::quat getRotation() const
 * glm::vec3 getScale() const
 */
class IBasicSceneObject : public ISceneObject, public IMovable, public IRotatable, public IScalable { };

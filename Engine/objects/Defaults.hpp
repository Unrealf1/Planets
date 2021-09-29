#pragma once

#include "render/RenderDefs.hpp"


const glm::vec3 default_position = glm::vec3(0.0f);
const glm::quat default_rotation = glm::toQuat(glm::lookAt(default_position, glm::vec3(0.0f, 0.0f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f)));
const glm::vec3 default_scale = glm::vec3(1.0f);

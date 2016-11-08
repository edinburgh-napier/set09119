#pragma once
#include "game.h"
#include "physics.h"
#include <glm/vec3.hpp>

namespace collision {
bool IsColliding(std::vector<collisionInfo> &civ, const cCollider &c1, const cCollider &c2);
}

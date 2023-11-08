#pragma once
#include "game.h"
#include <glm/glm.hpp>
#include <vector>

class cPhysics : public Component {
public:
  cPhysics();
  ~cPhysics();
  // virtual void Update(double delta);
  // virtual void SetParent(Entity *p);
  //
private:
};

class cCollider;
class cParticle;
struct collisionInfo {
  const cCollider *c1;
  const cCollider *c2;
  const glm::dvec3 position;
  const glm::dvec3 normal;
  const double depth;
};

void InitPhysics();
void ShutdownPhysics();
void UpdatePhysics(const double t, const double dt);

std::vector<cParticle *> &GetPhysicsScene();
std::vector<cCollider *> &GetColliders();
const glm::dvec3 &GetGravity();
// extern vector<cCollider *> colliders;
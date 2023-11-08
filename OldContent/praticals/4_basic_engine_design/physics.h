#pragma once
#include "game.h"

enum PHYSICSMODEL { SPHERE, BOX, POINT };

class cPhysics : public Component {
public:
  cPhysics();
  ~cPhysics();
  glm::vec3 position;
  glm::vec3 prev_position;
  double mass;
  const PHYSICSMODEL pm;
  void Update(double delta);
  void SetParent(Entity *p);

private:
};

void InitPhysics();
void ShutdownPhysics();
void UpdatePhysics(const double t, const double dt);

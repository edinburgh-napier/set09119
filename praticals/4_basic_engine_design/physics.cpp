#include "physics.h"
#include <glm/glm.hpp>
using namespace std;
using namespace glm;
static vector<cPhysics *> physicsScene;

static dvec3 gravity = dvec3(0, -10.0, 0);

cPhysics::cPhysics() : pm(POINT), Component("Physics") { physicsScene.push_back(this); }

cPhysics::~cPhysics() {
  auto position = std::find(physicsScene.begin(), physicsScene.end(), this);
  if (position != physicsScene.end()) {
    physicsScene.erase(position);
  }
}

void cPhysics::Update(double delta) {
  for (auto &e : physicsScene) {
    e->GetParent()->SetPosition(e->position);
  }
}

void cPhysics::SetParent(Entity *p) {
  Component::SetParent(p);
  position = Ent_->GetPosition();
  prev_position = position;
}

void UpdatePhysics(const double t, const double dt) {
  for (auto &e : physicsScene) {
    e->Render();
    // calcualte velocity from current and previous position
    dvec3 velocity = e->position - e->prev_position;
    // set previous position to current position
    e->prev_position = e->position;
    // position += v + a * (dt^2)
    e->position += velocity + gravity * pow(dt, 2);

    if (e->position.y <= 0.0f) {
      e->prev_position = e->position + (e->position - e->prev_position);
    }
  }
}

void InitPhysics() {}

void ShutdownPhysics() {}

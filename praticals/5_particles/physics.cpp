#include "physics.h"
#include "collision.h"
#include <glm/glm.hpp>
using namespace std;
using namespace glm;
static vector<cPhysics *> physicsScene;
static vector<cCollider *> colliders;

static dvec3 gravity = dvec3(0, -10.0, 0);

void Resolve(const collisionInfo &ci) {

  const double coef = 0.5;

  auto a = ci.c1->GetParent()->GetComponents("Physics");
  if (a.size() == 1) {
    const auto p = static_cast<cPhysics *>(a[0]);
    p->position += ci.normal * (ci.depth * 0.5);
    const double currentSpeed = glm::length(p->position - p->prev_position);
    p->prev_position = p->position + vec3(-ci.normal * currentSpeed * coef);
  }
  auto b = ci.c2->GetParent()->GetComponents("Physics");
  if (b.size() == 1) {
    const auto p = static_cast<cPhysics *>(b[0]);
    p->position += -ci.normal * (ci.depth * 0.5 * 0.1);
    const double currentSpeed = glm::length(p->position - p->prev_position);
    p->prev_position = p->position + vec3(ci.normal * currentSpeed * coef);
  }
}

cPhysics::cPhysics() : forces(dvec3(0)), mass(1.0), Component("Physics") { physicsScene.push_back(this); }

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

void cPhysics::AddImpulse(const glm::vec3 &i) { forces += i; }

void UpdatePhysics(const double t, const double dt) {
  std::vector<collisionInfo> collisions;
  // check for collisions
  {
    dvec3 pos;
    dvec3 norm;
    double depth;
    for (size_t i = 0; i < colliders.size(); ++i) {
      for (size_t j = i + 1; j < colliders.size(); ++j) {
        if (collision::IsColliding(*colliders[i], *colliders[j], pos, norm, depth)) {
          collisions.push_back({colliders[i], colliders[j], pos, norm, depth});
        }
      }
    }
  }
  // handle collisions
  {
    for (auto &c : collisions) {
      Resolve(c);
    }
  }
  // Integrate
  for (auto &e : physicsScene) {
    e->Render();
    // calcualte velocity from current and previous position
    dvec3 velocity = e->position - e->prev_position;
    // set previous position to current position
    e->prev_position = e->position;
    // position += v + a * (dt^2)
    e->position += velocity + (e->forces + gravity) * pow(dt, 2);
    e->forces = dvec3(0);
    if (e->position.y <= 0.0f) {
      //  e->prev_position = e->position + (e->position - e->prev_position);
    }
  }
}

void InitPhysics() {}

void ShutdownPhysics() {}
//----------------------

cParticle::cParticle() {}

cParticle::~cParticle() {}

void cParticle::Update(double delta) {}

//----------------------
cRigidBody::cRigidBody() {}

cRigidBody::~cRigidBody() {}

void cRigidBody::Update(double delta) {}

cCollider::cCollider(const std::string &tag) : Component(tag) { colliders.push_back(this); }

cCollider::~cCollider() {
  auto position = std::find(colliders.begin(), colliders.end(), this);
  if (position != colliders.end()) {
    colliders.erase(position);
  }
}

void cCollider::Update(double delta) {}

cSphereCollider::cSphereCollider() : radius(1.0), cCollider("SphereCollider") {}

cSphereCollider::~cSphereCollider() {}

cPlaneCollider::cPlaneCollider() : normal(dvec3(0, 1.0, 0)), cCollider("PlaneCollider") {}

cPlaneCollider::~cPlaneCollider() {}

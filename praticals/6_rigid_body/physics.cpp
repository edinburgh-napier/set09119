#include "physics.h"
#include "cPhysicsComponents.h"
#include "collision.h"

#include <glm/glm.hpp>
using namespace std;
using namespace glm;
static vector<cParticle *> physicsScene;
static vector<cCollider *> colliders;
static std::vector<collisionInfo> collisions;
static dvec3 gravity = dvec3(0, -10.0, 0);
const double coef = 0.5;
const double rigidcoef = 0.5;

void ResolveRB(cRigidBody *const b, const collisionInfo &ci, bool which) {
  // TODO: Fix.
  const double w = (which ? 1.0 : -1.0);
  dvec3 norm = ci.normal * w;

  dvec3 dv = b->position - b->prev_position;

  dvec3 R = ci.position - b->position;
  dvec3 Velocity = dv + cross(b->angVelocity, R);
  b->ComputeWorldInvInertia();
  double ImpulseNumerator = -(1.0 + rigidcoef) * dot(Velocity, norm);
  double ImpulseDenominator = b->inversemass + dot(cross(b->worldInvInertia * cross(R, norm), R), norm);

  double cheatfactor = 0.0; // stops sinking
  dvec3 Impulse = ((ImpulseNumerator / ImpulseDenominator) + (ci.depth * cheatfactor)) * norm;

  // apply impulse to primary quantities
  b->AddLinearImpulse(b->inversemass * Impulse);
  b->angMomentum += cross(R, Impulse);

  // compute affected auxiliary quantities
  b->angVelocity += b->worldInvInertia * cross(R, Impulse);
}

void ResolveP(cParticle *const b, const collisionInfo &ci, bool which) {
  const double w = (which ? -1.0 : 1.0);
  b->position += (w * ci.normal) * (ci.depth * 0.5);
  const double currentSpeed = glm::length(b->position - b->prev_position);
  b->prev_position = b->position + dvec3((-w * ci.normal) * currentSpeed * coef);
}

void Resolve(const collisionInfo &ci) {

  const auto r1 = ci.c1->GetParent()->getCompatibleComponent<cRigidBody>();
  const auto r2 = ci.c2->GetParent()->getCompatibleComponent<cRigidBody>();
  const auto p1 = ci.c1->GetParent()->getCompatibleComponent<cParticle>();
  const auto p2 = ci.c2->GetParent()->getCompatibleComponent<cParticle>();

  if (r1 != NULL) {
    ResolveRB(r1, ci, false);
  } else if (p1 != NULL) {
    ResolveP(p1, ci, false);
  }

  if (r2 != NULL) {
    ResolveRB(r2, ci, true);
  } else if (p2 != NULL) {
    ResolveP(p2, ci, true);
  }
}

void UpdatePhysics(const double t, const double dt) {
  collisions.clear();
  // check for collisions
  {
    for (size_t i = 0; i < colliders.size(); ++i) {
      for (size_t j = i + 1; j < colliders.size(); ++j) {
        collision::IsColliding(collisions, *colliders[i], *colliders[j]);
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
    e->Integrate(dt);
  }
}

void RenderPhysics() {
  for (auto &c : collisions) {
    phys::DrawLineCross(c.position, 0.5);
  }
}

std::vector<cParticle *> &GetPhysicsScene() { return physicsScene; }

std::vector<cCollider *> &GetColliders() { return colliders; }

const glm::dvec3 &GetGravity() { return gravity; }

void InitPhysics() {}

void ShutdownPhysics() {}
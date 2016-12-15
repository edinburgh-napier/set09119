#include "physics.h"
#include "collision.h"
#include <glm/glm.hpp>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

using namespace std;
using namespace glm;
static vector<cParticle *> physicsScene;
static vector<cCollider *> colliders;
static std::vector<collisionInfo> collisions;
static dvec3 gravity = dvec3(0, -10.0, 0);
const double coef = 0.99;
const double rigidcoef = 0.5;

//----------------------
cParticle::cParticle(const string &tag) : mass_(1.0), inversemass_(1.0), Component(tag) {
  GetPhysicsScene().push_back(this);
}

cParticle::~cParticle() {
  auto position = std::find(GetPhysicsScene().begin(), GetPhysicsScene().end(), this);
  if (position != GetPhysicsScene().end()) {
    GetPhysicsScene().erase(position);
  }
}

void cParticle::Update(double delta) {}

void cParticle::SetParent(Entity *p) {
  Component::SetParent(p);
  position_ = Ent_->GetPosition();
}

void cParticle::AddLinearForce(const glm::dvec3 &i) { forces_ += i; }

void cParticle::AddLinearImpulse(const glm::dvec3 &v) { velocity_ += v; }

void cParticle::Integrate(const double dt) {
  const dvec3 acceleration = (forces_ * inversemass_) + GetGravity();
  velocity_ += acceleration * dt;
  position_ += velocity_ * dt;
  forces_ = dvec3(0);
  GetParent()->SetPosition(position_);
}

void cParticle::Teleport(const glm::dvec3 &p) {
  position_ = p;
  GetParent()->SetPosition(position_);
}

const glm::dvec3 cParticle::GetPos() const { return position_; }

const glm::dvec3 cParticle::GetVel() const { return velocity_; }

const glm::dvec3 cParticle::GetForces() const { return forces_; }

cCollider::cCollider(const std::string &tag) : Component(tag), position_offset(dvec3(0)) {
  GetColliders().push_back(this);
}

cCollider::~cCollider() {
  auto position = std::find(GetColliders().begin(), GetColliders().end(), this);
  if (position != GetColliders().end()) {
    GetColliders().erase(position);
  }
}

void cCollider::Update(double delta) {}
cSphereCollider::cSphereCollider() : radius(1.0), DebugDraw(0), cCollider("SphereCollider") {}
void cSphereCollider::Render() {}

cPlaneCollider::cPlaneCollider()
    : normal_offset(dvec3(0, 1.0, 0)), DebugDraw(0), infinite(0), cCollider("PlaneCollider") {}
void cPlaneCollider::Render() {
  if (DebugDraw) {
    auto p = GetParent()->GetPosition() + position_offset;

    phys::DrawPlane(p, normal_offset, dvec3(0.02));
    auto transform = translate(dmat4(1.0f), p) * scale(dmat4(1.0f), GetParent()->GetScale()) *
                     mat4_cast(rotation(dvec3(0, 1.0, 0), normal_offset));
    // auto transform = translate(dmat4(1.0f), p);
    phys::DrawLineCross(dvec3(transform * dvec4(1.0f, 0.0f, -1.0f, 1)), 1.0f, false, AQUA);
    phys::DrawLineCross(dvec3(transform * dvec4(1.0f, 0.0f, 1.0f, 1)), 1.0f, false, GREEN);
    phys::DrawLineCross(dvec3(transform * dvec4(-1.0f, 0.0f, -1.0f, 1)), 1.0f, false, AQUA);
    phys::DrawLineCross(dvec3(transform * dvec4(-1.0f, 0.0f, 1.0f, 1)), 1.0f, false, GREEN);
  }
}

cBoxCollider::cBoxCollider() : radius(0.5), DebugDraw(0), cCollider("BoxCollider") {}

void cBoxCollider::Render() {}

void ResolveP(cParticle *const b, const collisionInfo &ci, bool which) {
  const double w = (which ? -1.0 : 1.0);
  auto reflection = b->GetVel() - 2.0 * ci.normal * (dot(b->GetVel(), ci.normal));
  b->Teleport(b->GetPos() + (w * ci.normal) * (ci.depth * 0.5));
  b->AddLinearImpulse(-b->GetVel() + (reflection * coef));
}

void Resolve(const collisionInfo &ci) {

  const auto p1 = ci.c1->GetParent()->getCompatibleComponent<cParticle>();
  const auto p2 = ci.c2->GetParent()->getCompatibleComponent<cParticle>();
  if (p1 != NULL) {
    ResolveP(p1, ci, false);
  }
  if (p2 != NULL) {
    ResolveP(p2, ci, true);
  }
}

namespace collision {
bool IsColliding(const cCollider &c1, const cCollider &c2, glm::dvec3 &pos, glm::dvec3 &norm, double &depth);
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

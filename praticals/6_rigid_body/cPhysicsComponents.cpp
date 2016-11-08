#include "cPhysicsComponents.h"
#include "collision.h"
#include "physics.h"
#include <glm/glm.hpp>
using namespace std;
using namespace glm;

//----------------------
cParticle::cParticle(const string &tag) : mass(1.0), inversemass(1.0), linearDamping(1.0), Component(tag) {
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
  position = Ent_->GetPosition();
  prev_position = position;
}

void cParticle::AddLinearForce(const glm::dvec3 &i) { forces += i; }

void cParticle::AddLinearImpulse(const glm::dvec3 &v) {
  const dvec3 dv = position - prev_position;
  const dvec3 newdv = dv + v;
  prev_position = position - newdv;
}

void cParticle::Integrate(const double dt) {
  // calcualte velocity from current and previous position
  dvec3 velocity = position - prev_position;
  // set previous position to current position
  prev_position = position;
  // position += v + a * (dt^2)
  position += velocity + ((forces + GetGravity()) * inversemass) * pow(dt, 2);

  forces = dvec3(0);
  GetParent()->SetPosition(position);
}

//----------------------
cRigidBody::cRigidBody() : angularDamping(0.9), orientation(normalize(dquat())), cParticle("RididBody") {
  ComputeLocalInvInertiaTensor();
}

cRigidBody::~cRigidBody() {}

void cRigidBody::Update(double delta) {}

void cRigidBody::AddForceAt(const glm::dvec3 &force, const glm::dvec3 &point) {
  // Add the force and torque
  forces += force;
  torques += cross(point - position, force);
}

void cRigidBody::AddAngularForce(const glm::dvec3 &i) { torques += i; }

void cRigidBody::ComputeLocalInvInertiaTensor() {
  localInvInertia = inverse(dmat3(1.0));
  worldInvInertia = mat4_cast(orientation) * dmat4(localInvInertia) * transpose(mat4_cast(orientation));
}

void cRigidBody::Integrate(const double dt) {
  // recycle linear stuff
  cParticle::Integrate(dt);

  angVelocity += worldInvInertia * torques * dt;
  angVelocity *= min(pow(angularDamping, dt), 1.0);

  orientation += dquat(angVelocity * dt) * orientation;
  orientation = normalize(orientation);

  torques = dvec3(0);
  GetParent()->SetRotation(orientation);
}

void cRigidBody::SetParent(Entity *p) {
  cParticle::SetParent(p);
  orientation = Ent_->GetRotation();
}

void cRigidSphere::ComputeLocalInvInertiaTensor() {
  if (inversemass == 0) {
    localInvInertia = dmat3(0.0);
    worldInvInertia = localInvInertia;
    return;
  }

  const double x2 = 4.0 * (radius * radius);
  const double ix = (x2 + x2) / (inversemass * 12.0);

  const double f = (1.0 / inversemass) * radius * radius * (2.0 / 5.0);
  dmat3 localInvInertia = dmat3(0.0);
  localInvInertia[0][0] = f;
  localInvInertia[1][1] = f;
  localInvInertia[2][2] = f;
  worldInvInertia = mat4_cast(orientation) * dmat4(localInvInertia) * transpose(mat4_cast(orientation));
}

void cRigidPlane::ComputeLocalInvInertiaTensor() {
  localInvInertia = mat3(0.0);
  worldInvInertia = localInvInertia;
}

void cRigidCube::ComputeLocalInvInertiaTensor() {
  if (inversemass == 0) {
    localInvInertia = dmat3(0.0);
    worldInvInertia = localInvInertia;
    return;
  }

  const double x2 = 4.0 * (radius * radius);
  const double ix = (x2 + x2) / (inversemass * 12.0);

  dmat3 localInvInertia = dmat3(0.0);
  localInvInertia[0][0] = 1.0 / ix;
  localInvInertia[1][1] = 1.0 / ix;
  localInvInertia[2][2] = 1.0 / ix;
  worldInvInertia = mat4_cast(orientation) * dmat4(localInvInertia) * transpose(mat4_cast(orientation));
}

cCollider::cCollider(const std::string &tag) : Component(tag) { GetColliders().push_back(this); }

cCollider::~cCollider() {
  auto position = std::find(GetColliders().begin(), GetColliders().end(), this);
  if (position != GetColliders().end()) {
    GetColliders().erase(position);
  }
}

void cCollider::Update(double delta) {}
cSphereCollider::cSphereCollider() : radius(1.0), cCollider("SphereCollider") {}
cPlaneCollider::cPlaneCollider() : normal(dvec3(0, 1.0, 0)), cCollider("PlaneCollider") {}
cBoxCollider::cBoxCollider() : radius(1.0), cCollider("BoxCollider") {}
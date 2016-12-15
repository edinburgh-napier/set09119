#pragma once
#include "game.h"
#include <glm/glm.hpp>
#include <vector>

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
void RenderPhysics();
std::vector<cParticle *> &GetPhysicsScene();
std::vector<cCollider *> &GetColliders();
const glm::dvec3 &GetGravity();

class cParticle : public Component {
public:
  cParticle(const std::string &tag = "Particle");
  ~cParticle();
  void Update(double delta);
  virtual void SetParent(Entity *p);
  virtual void AddLinearForce(const glm::dvec3 &i);
  virtual void AddLinearImpulse(const glm::dvec3 &i);
  virtual void Integrate(const double dt);
  virtual void Teleport(const glm::dvec3 &p);
  const glm::dvec3 GetPos() const;
  const glm::dvec3 GetVel() const;
  const glm::dvec3 GetForces() const;

private:
  glm::dvec3 position_;
  glm::dvec3 velocity_;
  glm::dvec3 forces_;
  double mass_;
  double inversemass_;
};

class cCollider : public Component {
public:
  cCollider(const std::string &tag);
  ~cCollider();
  glm::dvec3 position_offset;
  void Update(double delta);

private:
};

class cSphereCollider : public cCollider {

public:
  bool DebugDraw;
  double radius;
  cSphereCollider();
  virtual void Render();

private:
};

class cBoxCollider : public cCollider {
public:
  double radius;
  bool DebugDraw;
  cBoxCollider();
  virtual void Render();

private:
};

class cPlaneCollider : public cCollider {
public:
  glm::dvec3 normal_offset;
  bool DebugDraw;
  bool infinite;
  cPlaneCollider();
  virtual void Render();

private:
};

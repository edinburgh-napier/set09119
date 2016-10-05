#include "collision.h"
#include <glm/glm.hpp>

using namespace std;
using namespace glm;
namespace collision {

bool IsColliding(const cSphereCollider &c1, const cSphereCollider &c2, dvec3 &pos, dvec3 &norm, double &depth) {
  const dvec3 p1 = c1.GetParent()->GetPosition();
  const dvec3 p2 = c2.GetParent()->GetPosition();
  const dvec3 d = p2 - p1;
  const double distance = glm::length(d);
  const double sumRadius = c1.radius + c2.radius;
  if (distance < sumRadius) {
    depth = sumRadius - distance;
    norm = -glm::normalize(d);
    pos = p1 - norm * (c1.radius - depth * 0.5f);
    return true;
  }
  return false;
}

bool IsColliding(const cSphereCollider &s, const cPlaneCollider &p, dvec3 &pos, dvec3 &norm, double &depth) {
  const dvec3 sp = s.GetParent()->GetPosition();
  const dvec3 pp = p.GetParent()->GetPosition();

  // Calculate a vector from a point on the plane to the center of the sphere
  const dvec3 vecTemp(sp - pp);

  // Calculate the distance: dot product of the new vector with the plane's normal
  double distance = dot(vecTemp, p.normal);

  if (distance <= s.radius) {
    norm = p.normal;
    pos = sp - norm * distance;
    depth = s.radius - distance;
    return true;
  }

  return false;
}

bool IsColliding(const cPlaneCollider &c1, const cPlaneCollider &c2, dvec3 &pos, dvec3 &norm, double &depth) {
  cout << "PLANE PLANE" << endl;
  return false;
}

bool IsColliding(const cCollider &c1, const cCollider &c2, glm::dvec3 &pos, glm::dvec3 &norm, double &depth) {
  enum shape {
    UNKOWN = 0,
    PLANE,
    SPHERE,
  };
  shape s1 = UNKOWN;
  shape s2 = UNKOWN;
  if (dynamic_cast<const cSphereCollider *>(&c1)) {
    s1 = SPHERE;
  } else if (dynamic_cast<const cPlaneCollider *>(&c1)) {
    s1 = PLANE;
  }

  if (dynamic_cast<const cSphereCollider *>(&c2)) {
    s2 = SPHERE;
  } else if (dynamic_cast<const cPlaneCollider *>(&c2)) {
    s2 = PLANE;
  }

  if (!s1 || !s2) {
    cout << "Routing Error" << endl;
    return false;
  }
  if (s1 == PLANE) {
    if (s2 == PLANE) {
      return IsColliding(dynamic_cast<const cPlaneCollider &>(c1), dynamic_cast<const cPlaneCollider &>(c2), pos, norm,
                         depth);
    } else {
      return IsColliding(dynamic_cast<const cPlaneCollider &>(c1), dynamic_cast<const cSphereCollider &>(c2), pos, norm,
                         depth);
    }
  } else {
    if (s2 == PLANE) {
      return IsColliding(dynamic_cast<const cSphereCollider &>(c1), dynamic_cast<const cPlaneCollider &>(c2), pos, norm,
                         depth);
    } else {
      return IsColliding(dynamic_cast<const cSphereCollider &>(c1), dynamic_cast<const cSphereCollider &>(c2), pos,
                         norm, depth);
    }
  }
}
}
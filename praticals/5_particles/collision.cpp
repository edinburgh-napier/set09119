#include "collision.h"
#include "physics.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
using namespace std;
using namespace glm;
namespace collision {

bool IsCollidingCheck(std::vector<collisionInfo> &civ, const cSphereCollider &c1, const cSphereCollider &c2) {
  const dvec3 p1 = c1.GetParent()->GetPosition();
  const dvec3 p2 = c2.GetParent()->GetPosition();
  const dvec3 d = p2 - p1;
  const double distance = glm::length(d);
  const double sumRadius = c1.radius + c2.radius;
  if (distance < sumRadius) {
    auto depth = sumRadius - distance;
    auto norm = -glm::normalize(d);
    auto pos = p1 - norm * (c1.radius - depth * 0.5f);
    civ.push_back({&c1, &c2, pos, norm, depth});
    return true;
  }
  return false;
}

bool IsCollidingCheck(std::vector<collisionInfo> &civ, const cSphereCollider &s, const cPlaneCollider &p) {
  const dvec3 sp = s.GetParent()->GetPosition();
  const dvec3 pp = p.GetParent()->GetPosition() + p.position_offset;

  // Calculate a vector from a point on the plane to the center of the sphere
  const dvec3 vecTemp(sp - pp);

  // Calculate the distance: dot product of the new vector with the plane's normal
  double distance = dot(vecTemp, p.normal_offset);

  if (distance <= s.radius) {
    if (p.infinite) {
      civ.push_back({&s, &p, sp - p.normal_offset * distance, p.normal_offset, s.radius - distance});
      return true;
    } else {

      const auto  max_extents = length(dvec2(p.GetParent()->GetScale().x, p.GetParent()->GetScale().z));


      if (length(vecTemp) - s.radius > max_extents) {
        return false;
      }

      auto transform = translate(dmat4(1.0f), pp) * scale(dmat4(1.0f), p.GetParent()->GetScale()) *
                       mat4_cast(rotation(dvec3(0, 1.0, 0), p.normal_offset));

      dvec3 A =dvec3(dvec4(1.0f,0.0f,-1.0f,0) *transform) -sp;
      dvec3 B =dvec3(dvec4(1.0f,0.0f,1.0f, 0) *transform) -sp;
      dvec3 C =dvec3(dvec4(-1.0f,0.0f,-1.0f, 0) *transform) -sp;
      dvec3 D =dvec3(dvec4(-1.0f,0.0f,1.0f, 0) *transform) -sp;
      auto rr = s.radius * s.radius;
      auto aa = dot(A, A);
      auto ab = dot(A, B);
      auto ac = dot(A, C);
      auto bb = dot(B, B);
      auto bc = dot(B, C);
      auto cc = dot(C, C);

      if ((aa > rr) && (ab > aa) && (ac > aa)) { 
    //    civ.push_back({ &s, &p, sp - p.normal_offset * distance, p.normal_offset, s.radius - distance });
        return true; }
      if ((bb > rr) && (ab > bb) && (bc > bb)) { 
    //    civ.push_back({ &s, &p, sp - p.normal_offset * distance, p.normal_offset, s.radius - distance });
        return true; }
      if ((cc > rr) && (ac > cc) && (bc > cc)) {
      //  civ.push_back({ &s, &p, sp - p.normal_offset * distance, p.normal_offset, s.radius - distance });
        return true; }

      auto AB = B - A;
      auto BC = C - B;
      auto CA = A - C;
      auto d1 = ab - aa;
      auto d2 = bc - bb;
      auto d3 = ac - cc;
      auto e1 = dot(AB, AB);
      auto e2 = dot(BC, BC);
      auto e3 = dot(CA, CA);
      auto Q1 = A * e1 - d1 * AB;
      auto Q2 = B * e2 - d2 * BC;
      auto Q3 = C * e3 - d3 * CA;
      auto QC = C * e1 - Q1;
      auto QA = A * e2 - Q2;
      auto QB = B * e3 - Q3;

      if ((dot(Q1, Q1) > rr * e1 * e1) && (dot(Q1, QC) > 0)) { 
        civ.push_back({ &s, &p, sp - p.normal_offset * distance, p.normal_offset, s.radius - distance });
        return true; }
      if ((dot(Q2, Q2) > rr * e2 * e2) && (dot(Q2, QA) > 0)) { 
        civ.push_back({ &s, &p, sp - p.normal_offset * distance, p.normal_offset, s.radius - distance });
        return true; }
      if ((dot(Q3, Q3) > rr * e3 * e3) && (dot(Q3, QB) > 0)) { 
        civ.push_back({ &s, &p, sp - p.normal_offset * distance, p.normal_offset, s.radius - distance });
        return true; 
      }

    }
  }

  return false;
}

bool IsCollidingCheck(std::vector<collisionInfo> &civ, const cSphereCollider &c1, const cBoxCollider &c2) {
  const dvec3 sp = c1.GetParent()->GetPosition();
  const dvec3 bp = c2.GetParent()->GetPosition();
  if (length(sp - bp) < c1.radius + c2.radius) {
    // TODO
    cout << "Sphere Box" << endl;
    // return true;
  }

  return false;
}

bool IsCollidingCheck(std::vector<collisionInfo> &civ, const cPlaneCollider &c1, const cPlaneCollider &c2) {

  // easy out, are normals perendicular?
  auto pp = cross(c1.normal_offset, c2.normal_offset);
  if (!pp.x && !pp.y && !pp.x) {
    // unless they are the same height
    if (c1.GetParent()->GetPosition() + c1.position_offset == c2.GetParent()->GetPosition() + c2.position_offset) {
      // civ.push_back({ &c1, &c2, pos, norm, depth }); //Todo
      return true;
    }
    return false;
  }
  // not parralel, are any of them infinite
  if (c1.infinite || c2.infinite) {
    // civ.push_back({ &c1, &c2, pos, norm, depth }); //Todo
    return true;
  }
  // Neither are infinite, are they far enough apart that they couldn't posibly collide?
  {
    const auto dist =
        length(c1.GetParent()->GetPosition() + c1.position_offset - c2.GetParent()->GetPosition() + c2.position_offset);
    if (dist > length(c1.GetParent()->GetScale() + c2.GetParent()->GetScale())) {
      // civ.push_back({ &c1, &c2, pos, norm, depth }); //Todo
      return false;
    }
  }
  // TODO
  cout << "PLANE PLANE" << endl;
  return false;
}

bool IsCollidingCheck(std::vector<collisionInfo> &civ, const cPlaneCollider &p, const cBoxCollider &b) {
  const dvec3 pp = p.GetParent()->GetPosition();
  const dvec3 bp = b.GetParent()->GetPosition();

  // local coords on cube
  dvec3 points[8] = {dvec3(b.radius, b.radius, b.radius),   dvec3(-b.radius, b.radius, b.radius),
                     dvec3(b.radius, -b.radius, b.radius),  dvec3(-b.radius, -b.radius, b.radius),
                     dvec3(b.radius, b.radius, -b.radius),  dvec3(-b.radius, b.radius, -b.radius),
                     dvec3(b.radius, -b.radius, -b.radius), dvec3(-b.radius, -b.radius, -b.radius)};

  // transfrom to global
  const mat4 m = glm::translate(bp) * mat4_cast(b.GetParent()->GetRotation());
  for (int i = 0; i < 8; i++) {
    points[i] = dvec3(m * dvec4(points[i], 1.0));
  }

  // For each point on the cube, which side of cube are they on?
  double distances[8];
  bool isCollided = false;
  for (int i = 0; i < 8; i++) {
    dvec3 planeNormal = p.normal_offset;

    distances[i] = dot(pp, planeNormal) - dot(points[i], planeNormal);

    if (distances[i] > 0) {
      //	 cout << "CuboidPlane!\n";
      civ.push_back({&p, &b, points[i] + planeNormal * distances[i], planeNormal, distances[i]});
      isCollided = true;
    }
  }
  return isCollided;
}

bool IsCollidingCheck(std::vector<collisionInfo> &civ, const cBoxCollider &c1, const cBoxCollider &c2) {
  cout << "Box Box" << endl;
  return false;
}

bool IsColliding(std::vector<collisionInfo> &civ, const cCollider &c1, const cCollider &c2) {
  enum shape { UNKOWN = 0, PLANE, SPHERE, BOX };
  shape s1 = UNKOWN;
  shape s2 = UNKOWN;
  if (dynamic_cast<const cSphereCollider *>(&c1)) {
    s1 = SPHERE;
  } else if (dynamic_cast<const cPlaneCollider *>(&c1)) {
    s1 = PLANE;
  } else if (dynamic_cast<const cBoxCollider *>(&c1)) {
    s1 = BOX;
  }

  if (dynamic_cast<const cSphereCollider *>(&c2)) {
    s2 = SPHERE;
  } else if (dynamic_cast<const cPlaneCollider *>(&c2)) {
    s2 = PLANE;
  } else if (dynamic_cast<const cBoxCollider *>(&c2)) {
    s2 = BOX;
  }

  if (!s1 || !s2) {
    cout << "Routing Error" << endl;
    return false;
  }
  if (s1 == PLANE) {
    if (s2 == PLANE) {
      return IsCollidingCheck(civ, dynamic_cast<const cPlaneCollider &>(c1), dynamic_cast<const cPlaneCollider &>(c2));
    } else if (s2 == SPHERE) {
      return IsCollidingCheck(civ, dynamic_cast<const cSphereCollider &>(c2), dynamic_cast<const cPlaneCollider &>(c1));
    } else if (s2 == BOX) {
      return IsCollidingCheck(civ, dynamic_cast<const cPlaneCollider &>(c1), dynamic_cast<const cBoxCollider &>(c2));
    } else {
      cout << "Routing Error" << endl;
      return false;
    }
  } else if (s1 == SPHERE) {
    if (s2 == PLANE) {
      return IsCollidingCheck(civ, dynamic_cast<const cSphereCollider &>(c1), dynamic_cast<const cPlaneCollider &>(c2));
    } else if (s2 == SPHERE) {
      return IsCollidingCheck(civ, dynamic_cast<const cSphereCollider &>(c1),
                              dynamic_cast<const cSphereCollider &>(c2));
    } else if (s2 == BOX) {
      return IsCollidingCheck(civ, dynamic_cast<const cSphereCollider &>(c1), dynamic_cast<const cBoxCollider &>(c2));
    } else {
      cout << "Routing Error" << endl;
      return false;
    }
  } else if (s1 == BOX) {
    if (s2 == PLANE) {
      return IsCollidingCheck(civ, dynamic_cast<const cPlaneCollider &>(c2), dynamic_cast<const cBoxCollider &>(c1));
    } else if (s2 == SPHERE) {
      return IsCollidingCheck(civ, dynamic_cast<const cSphereCollider &>(c2), dynamic_cast<const cBoxCollider &>(c1));
    } else if (s2 == BOX) {
      return IsCollidingCheck(civ, dynamic_cast<const cBoxCollider &>(c2), dynamic_cast<const cBoxCollider &>(c1));
    } else {
      cout << "Routing Error" << endl;
      return false;
    }
  }
  return false;
}
}
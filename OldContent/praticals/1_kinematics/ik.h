#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <vector>

class Link {
public:
  Link(glm::dvec3 &axis, double angle) {
    m_axis = axis;
    m_angle = angle;
  }
  // Note- we could store this as a quaternion
  glm::dvec3 m_axis; // local axis
  double m_angle;    // local angle

  glm::dvec3 m_worldaxis;
  glm::dmat4 m_base;
  glm::dmat4 m_end;
};

void UpdateHierarchy();
void ik_1dof_Update(const glm::dvec3 &target, std::vector<Link> &links, const double linkLength);
void ik_3dof_Update(const glm::dvec3 &target, std::vector<Link> &links, const double linkLength);
void ik_jacob_Update(const glm::dvec3 &target, std::vector<Link> &links, const double linkLength);
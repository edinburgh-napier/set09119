#pragma once
#include <glm/glm.hpp>
#include <vector>

class Link {
public:
  Link(glm::vec3 &axis, float angle) {
    m_axis = axis;
    m_angle = angle;
  }
  // Note- we could store this as a quaternion
  glm::vec3 m_axis; // local axis
  float m_angle;    // local angle

  glm::vec3 m_worldaxis;
  glm::mat4 m_base;
  glm::mat4 m_end;
};

void UpdateHierarchy();
void ik_1dof_Update(const glm::vec3 &const target, std::vector<Link> &const links, const float linkLength);
void ik_3dof_Update(const glm::vec3 &const target, std::vector<Link> &const links, const float linkLength);
void ik_jacob_Update(const glm::vec3 &const target, std::vector<Link> &const links, const float linkLength);
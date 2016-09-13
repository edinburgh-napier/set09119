#include "ik.h"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <iostream>
#include <vector>
using namespace std;
using namespace glm;
static unsigned int numLinks = 0;

static void Reach(int i, const vec3 &target, std::vector<Link> &const links) {
  // our current orientation
  dquat qCur = angleAxis(links[i].m_angle, links[i].m_axis);
  // current position of this effector
  vec3 vlinkBasePos = (links[i].m_base)[3];
  vec3 vLinkEndPos = (links[i].m_end)[3];
  // current position of the effector at the end of the chain
  vec3 vEndEffPos = links[links.size() - 1].m_end[3];
  if ((i < numLinks - 2) && length(target - vLinkEndPos) < 1.0f) {
    // I'm moving my next link too close to the target, orbit around
    links[i].m_angle -= 0.1f;
    vec3 oaxis = normalize(target - vlinkBasePos);
    links[i].m_axis = rotate(links[i].m_axis, 0.1f, oaxis);
    return;
  }

  // These are the two vectors we want to converge.
  vec3 vLinkBaseToEndEffDirection = normalize(vEndEffPos - vlinkBasePos);
  vec3 vLinkBaseToTargetDirection = normalize(target - vlinkBasePos);

  // Get Dot of the two vectors
  float cosAngle = dot(vLinkBaseToTargetDirection, vLinkBaseToEndEffDirection);
  if (abs(cosAngle) < 1.0f) {
    // *********************************
    // Get the Axis perpendicular to the two vectors

    // Get the Angle between the two vectors

    // Turn into a Quat

    // Multply our current Quat with it

    // Pull out the angle and axis components, set the link params


    // *********************************
  }
}

void ik_3dof_Update(const vec3 &const target, std::vector<Link> &const links, const float linkLength) {
  numLinks = links.size();
  for (size_t i = links.size(); i >= 1; --i) {
    // for (int i = 0; i < links.size(); i++) {
    UpdateHierarchy();
    Reach(i - 1, target, links);
    const float distance = length(vec3(links[links.size() - 1].m_end[3]) - target);
    if (distance < 0.5f) {
      return;
    }
  }
}
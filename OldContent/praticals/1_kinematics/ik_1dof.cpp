#include "ik.h"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <iostream>
#include <phys_utils.h>
#include <vector>
using namespace std;
using namespace glm;

static void Reach(const size_t i, const dvec3 &target, std::vector<Link> &links) {
  // our current orientation
  dquat qCur = angleAxis(links[i].m_angle, links[i].m_axis);
  // current position of this effector
  vec3 vlinkBasePos = (links[i].m_base)[3];
  // current position of the effector at the end of the chain
  vec3 vEndEffPos = links[links.size() - 1].m_end[3];
  // convert our axis to world space
  vec3 vLinkAxis = links[i].m_worldaxis;
  // project target onto axis plane
  vec3 vplanetarget = projectOntoPlane(target, vLinkAxis, vlinkBasePos);
  // project vEndEffPos onto axis plane
  vec3 vplaneEndEff = projectOntoPlane(vEndEffPos, vLinkAxis, vlinkBasePos);

  // These are the two vectors we want to converge.
  vec3 vLinkBaseToEndEffDirection = normalize(vplaneEndEff - vlinkBasePos);
  vec3 vLinkBaseToTargetDirection = normalize(vplanetarget - vlinkBasePos);

  // Get Dot of the two vectors
  float cosAngle = dot(vLinkBaseToEndEffDirection, vLinkBaseToTargetDirection);
  if (abs(cosAngle) < 1.0f) {
    // *********************************
    // Get the Angle between the two vectors


    // Turn into a Quat with our axis

    // Multply our current Quat with it

    // Pull out the angle component, set the link params

    // *********************************
  }
}

void ik_1dof_Update(const dvec3 & target, std::vector<Link> &links, const double linkLength) {
  // for (size_t i = links.size(); i >= 1; --i) {
  for (size_t i = 0; i < links.size(); ++i) {
    UpdateHierarchy();
    Reach(i, target, links);
    const double distance = length(dvec3(links[links.size() - 1].m_end[3]) - target);
    if (distance < 0.5f) {
      return;
    }
  }
}
#include "ik.h"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <graphics_framework.h>
#include <phys_utils.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

// Simulation constants
int numLinks = 3;
double linkLength = 2.0f; // Length of each link
std::vector<Link> links;
dvec3 target = dvec3(4.0f, 4.0f, 0);

void MoveTarget() {
  target = glm::ballRand((static_cast<float>(numLinks) * linkLength) * 0.6f);
  target.y = abs(target.y);
  // target.x = abs(target.x);
  cout << "boop" << endl;
}

void UpdateHierarchy() {
  for (int i = 0; i < (int)links.size(); ++i) {
    dmat4 R1 = mat4_cast(angleAxis(links[i].m_angle, links[i].m_axis));
    dmat4 T1 = translate(dmat4(1.0f), dvec3(linkLength, 0, 0));
    links[i].m_base = dmat4(1.0) * R1;
    links[i].m_end = links[i].m_base * T1;
    links[i].m_worldaxis = links[i].m_axis;
    if (i > 0) {
      // Don't move the root link.
      links[i].m_base = links[i - 1].m_end * links[i].m_base;
      links[i].m_end = links[i].m_base * links[i].m_end;
      links[i].m_worldaxis = normalize(mat3(links[i - 1].m_end) * links[i].m_axis);
    }
  }
}

bool load_content() {
  phys::Init();
  for (int i = 0; i < numLinks; ++i) {
    int ax = (i + 0) % 3 == 0;
    int ay = (i + 1) % 3 == 0;
    int az = (i + 2) % 3 == 0;
    links.push_back(Link(dvec3(ax, ay, az), 0.0));
  }
  UpdateHierarchy();
  return true;
}

void UpdateIK() {
  UpdateHierarchy();
  const double distance = length(dvec3(links[links.size() - 1].m_end[3]) - target);
  if (distance < 0.5) {
    MoveTarget();
  }
  //ik_1dof_Update(target, links, linkLength);
  ik_3dof_Update(target, links, linkLength);
}

void RenderIK() {
  phys::DrawSphere(target, 0.2f, RED);
  for (int i = 0; i < (int)links.size(); ++i) {
    dvec3 base = links[i].m_base[3];
    dvec3 end = links[i].m_end[3];
    phys::DrawCube(links[i].m_base * glm::scale(dmat4(1.0), dvec3(0.5)), GREEN);
    phys::DrawCube(links[i].m_end * glm::scale(dmat4(1.0), dvec3(0.5)), ORANGE);
    phys::DrawLine(base, end);
    phys::DrawPlane(base, links[i].m_worldaxis, dvec3(0.01));
  }
}

bool update(float delta_time) {
  static float rot = 0.0f;
  rot += 0.2f * delta_time;
  phys::SetCameraPos(rotate(vec3(15.0f, 12.0f, 15.0f), rot, vec3(0, 1.0f, 0)));
  UpdateIK();
  phys::Update(delta_time);
  return true;
}

bool render() {
  RenderIK();
  phys::DrawScene();
  return true;
}

void main() {
  // Create application
  app application;
  // Set load content, update and render methods
  application.set_load_content(load_content);
  application.set_update(update);
  application.set_render(render);
  // Run application
  application.run();
}
#include <glm/glm.hpp>
#include <vector>

using namespace glm;

#define physics_tick 1.0 / 60.0

void UpdatePhysics_Euler(const double t, const double dt);
void UpdatePhysics_Verlet(const double t, const double dt);
void UpdatePhysics_rk4(const double t, const double dt);

struct sBall {
  glm::dvec3 velocity;
  glm::dvec3 position;
  glm::dvec3 position_prev; //not all integrations use this
};

extern std::vector<sBall> balls;
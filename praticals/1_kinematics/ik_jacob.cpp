#include "ik.h"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <vector>
using namespace std;
using namespace glm;

static void ik_jacob_Update(const glm::dvec3 &target, std::vector<Link> &links, const double linkLength) {}
#include "ik.h"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <vector>
using namespace std;
using namespace glm;

static void ik_jacob_Update(const glm::vec3 &const target, std::vector<Link> &const links, const float linkLength) {}
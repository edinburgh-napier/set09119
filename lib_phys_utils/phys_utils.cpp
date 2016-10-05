#include "phys_utils.h"
#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

namespace phys {
target_camera cam;
effect effP;
effect effB;
effect effG;
glm::mat4 PV;
directional_light light;
material mat;

void Init() {

  effB = effect();
  effB.add_shader("shaders/phys_basic.vert", GL_VERTEX_SHADER);
  effB.add_shader("shaders/phys_basic.frag", GL_FRAGMENT_SHADER);
  effB.build();
  effP = effect();
  effP.add_shader("shaders/phys_phong.vert", GL_VERTEX_SHADER);
  effP.add_shader("shaders/phys_phong.frag", GL_FRAGMENT_SHADER);
  effP.build();
  effG = effect();
  effG.add_shader("shaders/phys_grid.vert", GL_VERTEX_SHADER);
  effG.add_shader("shaders/phys_grid.frag", GL_FRAGMENT_SHADER);
  effG.build();
  cam.set_position(vec3(10.0f, 10.0f, 10.0f));
  cam.set_target(vec3(0.0f, 0.0f, 0.0f));
  auto aspect = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());
  cam.set_projection(quarter_pi<float>(), aspect, 2.414f, 1000.0f);

  light.set_ambient_intensity(vec4(0.5f, 0.5f, 0.5f, 1.0f));
  light.set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
  light.set_direction(vec3(0.0f, 1.0f, 0.0f));
  mat = material(vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), 25.0f);
}

void Update(double delta_time) {
  PV = cam.get_projection() * cam.get_view();
  cam.update(static_cast<float>(delta_time));
  renderer::setClearColour(0, 0, 0);
}

const glm::vec3 UP(0, 1.0f, 0);
void DrawArrow(const glm::vec3 &p0, const glm::vec3 &p1, const double thickness, const RGBAInt32 col) {
  static geometry tube = geometry_builder::create_cylinder();
  static geometry cone = geometry_builder::create_pyramid();
  const auto line = p1 - p0;
  const auto direction = glm::normalize(line);
  const auto length = glm::length(line);
  const auto l1 = length * 0.7f;
  const auto l2 = length * 0.3f;
  const auto midp = p0 + (l1 * direction);
  const mat4 tubeScale = glm::scale(mat4(1.0f), vec3(1.0f, l1, 1.0f));
  const mat4 tubeTrn = glm::translate(mat4(1.0f), p0 + ((l1 * 0.5f) * direction));
  const mat4 coneScale = glm::scale(mat4(1.0f), vec3(1.0f, l2, 1.0f));
  const mat4 coneTrn = glm::translate(mat4(1.0f), midp + ((l2 * 0.5f) * direction));

  auto RotationAxis = glm::cross(UP, direction);
  auto RotationAngle = glm::acos(dot(UP, direction));
  auto Rotation = glm::mat4_cast(glm::angleAxis(RotationAngle, RotationAxis));
  mat3 N(1.0f);
  mat.set_diffuse(col.tovec4());
  renderer::bind(effP);
  renderer::bind(mat, "mat");
  renderer::bind(light, "light");
  glUniformMatrix3fv(effP.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));

  auto M = tubeTrn * Rotation * tubeScale;
  glUniformMatrix4fv(effP.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
  glUniformMatrix4fv(effP.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(PV * M));
  renderer::render(tube);

  M = coneTrn * Rotation * coneScale;
  glUniformMatrix4fv(effP.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
  glUniformMatrix4fv(effP.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(PV * M));
  renderer::render(cone);
}

void DrawLine(const glm::vec3 &p0, const glm::vec3 &p1, const bool depth, const RGBAInt32 col) {
  renderer::bind(effB);
  static bool ready = false;
  static unsigned int vao;
  static unsigned int vbo;
  if (!ready) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    ready = true;
  }

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glm::vec3 line[2] = {p0, p1};
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 2, &line[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), NULL);

  GLfloat colour[4];
  col.tofloat(colour);
  glUniformMatrix4fv(effB.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(PV));
  glUniform4fv(effB.get_uniform_location("colour_override"), 1, &colour[0]);
  if (!depth) {
    glDisable(GL_DEPTH_TEST);
  }
  glDrawArrays(GL_LINES, 0, 2);
  if (!depth) {
    glEnable(GL_DEPTH_TEST);
  }
  glBindVertexArray(NULL);
}

void DrawSphere(const glm::vec3 &p0, float radius, const RGBAInt32 col) {
  static geometry geom = geometry_builder::create_sphere();
  renderer::bind(effP);
  auto M = glm::translate(mat4(1.0f), p0) * glm::scale(mat4(1.0f), vec3(radius));
  auto MVP = PV * M;
  mat3 N(1.0f);
  mat.set_diffuse(col.tovec4());
  renderer::bind(mat, "mat");
  renderer::bind(light, "light");
  glUniformMatrix4fv(effP.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
  glUniformMatrix4fv(effP.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
  glUniformMatrix3fv(effP.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));
  renderer::render(geom);
}

void DrawCube(const glm::vec3 &p0, const glm::vec3 &scale, const RGBAInt32 col) {
  static geometry geom = geometry_builder::create_box();
  renderer::bind(effP);
  auto M = glm::translate(mat4(1.0f), p0) * glm::scale(mat4(1.0f), scale);
  mat3 N(1.0f);
  mat.set_diffuse(col.tovec4());
  renderer::bind(mat, "mat");
  renderer::bind(light, "light");
  glUniformMatrix4fv(effP.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(PV * M));
  glUniformMatrix4fv(effP.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
  glUniformMatrix3fv(effP.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));
  renderer::render(geom);
}

void DrawCube(const glm::mat4 &m, const RGBAInt32 col) {
  static geometry geom = geometry_builder::create_box();
  renderer::bind(effP);
  auto M = m;
  mat3 N(1.0f);
  mat.set_diffuse(col.tovec4());
  renderer::bind(mat, "mat");
  renderer::bind(light, "light");
  glUniformMatrix4fv(effP.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(PV * M));
  glUniformMatrix4fv(effP.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
  glUniformMatrix3fv(effP.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));
  renderer::render(geom);
}

void DrawLineCross(const glm::vec3 &p0, float radius, const bool depth, const RGBAInt32 col) {
  DrawLine(p0 + glm::vec3(radius, 0, 0), p0 - glm::vec3(radius, 0, 0), depth, col);
  DrawLine(p0 + glm::vec3(0, radius, 0), p0 - glm::vec3(0, radius, 0), depth, col);
  DrawLine(p0 + glm::vec3(0, 0, radius), p0 - glm::vec3(0, 0, radius), depth, col);
}

void DrawPlane(const glm::vec3 &p0, const glm::vec3 &norm, const glm::vec3 &scale, const RGBAInt32 col) {
  static geometry geom = geometry_builder::create_plane();
  renderer::bind(effP);
  auto M =
      glm::translate(mat4(1.0f), p0) * glm::scale(mat4(1.0f), scale) * mat4_cast(glm::rotation(vec3(0, 1.0, 0), norm));
  mat3 N(1.0f);
  mat.set_diffuse(col.tovec4());
  renderer::bind(mat, "mat");
  renderer::bind(light, "light");
  glUniformMatrix4fv(effP.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(PV * M));
  glUniformMatrix4fv(effP.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
  glUniformMatrix3fv(effP.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));
  glDisable(GL_CULL_FACE);
  renderer::render(geom);
  glEnable(GL_CULL_FACE);
}

void SetCameraPos(const glm::vec3 &p0) {
  cam.set_position(p0);
  PV = cam.get_projection() * cam.get_view();
}

void SetCameraTarget(const glm::vec3 &p0) {
  cam.set_target(p0);
  PV = cam.get_projection() * cam.get_view();
}

void RGBAInt32::tofloat(float *const arr) const {
  arr[0] = static_cast<float>(b[0]) / 255.0f;
  arr[1] = static_cast<float>(b[1]) / 255.0f;
  arr[2] = static_cast<float>(b[2]) / 255.0f;
  arr[3] = static_cast<float>(b[3]) / 255.0f;
}
glm::vec4 RGBAInt32::tovec4() const {
  return glm::vec4(static_cast<float>(b[0]) / 255.0f, static_cast<float>(b[1]) / 255.0f,
                   static_cast<float>(b[2]) / 255.0f, static_cast<float>(b[3]) / 255.0f);
}

const RGBAInt32 RandomColour() {
  RGBAInt32 c;
  c.b[0] = rand() % 256;
  c.b[1] = rand() % 256;
  c.b[2] = rand() % 256;
  c.b[3] = 255;
  return c;
}

void DrawScene() {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendEquation(GL_FUNC_ADD);
  static geometry geom = geometry_builder::create_plane();
  renderer::bind(effG);
  auto M = glm::scale(mat4(1.0f), vec3(10.0f, 1.0, 10.0f));
  mat3 N(1.0f);
  RGBAInt32 col = GREY;
  mat.set_diffuse(col.tovec4());
  renderer::bind(mat, "mat");
  renderer::bind(light, "light");
  glUniformMatrix4fv(effG.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(PV * M));
  glUniformMatrix4fv(effG.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
  glUniformMatrix3fv(effG.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));
  renderer::render(geom);
}
}

glm::vec3 projectOntoPlane(const glm::vec3 &point, const glm::vec3 &planeNormal, const glm::vec3 &planePosition) {
  if (!(planeNormal.length() > 0.0f)) {
    return vec3(0);
  }
  vec3 q_proj = point - dot(point - planePosition, planeNormal) * planeNormal;
  return (q_proj);
}

std::ostream &operator<<(std::ostream &out, const glm::vec3 &v) {
  out << "(" << v.x << "," << v.y << "," << v.z << ")";
  return out;
}
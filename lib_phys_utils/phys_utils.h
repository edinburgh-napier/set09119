#pragma once
#include <glm\glm.hpp>
#include <iostream>
#define RED                                                                                                            \
  { 4278190335 }
#define GREEN                                                                                                          \
  { 4278255360 }
#define BLUE                                                                                                           \
  { 4294901760 }
#define BLACK                                                                                                          \
  { 4278190080 }
#define WHITE                                                                                                          \
  { 4294967295 }
#define ORANGE                                                                                                         \
  { 4278223103 }
#define YELLOW                                                                                                         \
  { 4278255615 }
#define PINK                                                                                                           \
  { 4294902015 }
#define AQUA                                                                                                           \
  { 4294967040 }
#define GREY                                                                                                           \
  { 4286611584 }

namespace phys {

union RGBAInt32 {
  uint32_t i;
  unsigned char b[4];
  void tofloat(float *const arr) const;
  glm::vec4 RGBAInt32::tovec4() const;
};

const RGBAInt32 RandomColour();
void DrawScene();
void Init();
void Update(double delta_time);
void SetCameraPos(const glm::vec3 &p0);
void SetCameraTarget(const glm::vec3 &p0);
void DrawLine(const glm::vec3 &p0, const glm::vec3 &p1, const bool depth = true, const RGBAInt32 col = RED);
void DrawLineCross(const glm::vec3 &p0, float radius = 1.0f, const bool depth = true, const RGBAInt32 col = RED);

void DrawPlane(const glm::vec3 &p0, const glm::vec3 &norm, const glm::vec3 &scale = glm::vec3(1.0f, 1.0f, 1.0f),
               const RGBAInt32 col = RED);

void DrawArrow(const glm::vec3 &p0, const glm::vec3 &p1, const double thickness, const RGBAInt32 col = RED);
void DrawSphere(const glm::vec3 &p0, float radius = 1.0f, const RGBAInt32 col = RED);
void DrawCube(const glm::vec3 &p0, const glm::vec3 &scale = glm::vec3(1.0f, 1.0f, 1.0f), const RGBAInt32 col = RED);
void DrawCube(const glm::mat4 &m, const RGBAInt32 col = RED);
}

glm::vec3 projectOntoPlane(const glm::vec3 &point, const glm::vec3 &planeNormal,
                           const glm::vec3 &planePosition = glm::vec3(0));

std::ostream &operator<<(std::ostream &out, const glm::vec3 &v);
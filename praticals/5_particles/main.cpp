#include "game.h"
#include "physics.h"
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <graphics_framework.h>
#include <phys_utils.h>
#include <thread>

using namespace std;
using namespace graphics_framework;
using namespace glm;
#define physics_tick 1.0 / 60.0

static vector<unique_ptr<Entity>> SceneList;

bool update(double delta_time) {
  static double t = 0.0;
  static double accumulator = 0.0;
  accumulator += delta_time;

  while (accumulator > physics_tick) {
    UpdatePhysics(t, physics_tick);
    accumulator -= physics_tick;
    t += physics_tick;
  }

  for (auto &e : SceneList) {
    e->Update(delta_time);
  }

  phys::Update(delta_time);
  return true;
}

bool load_content() {
  phys::Init();
  // Ball Factory
  for (size_t i = 0; i < 10; i++) {
    unique_ptr<Entity> ent(new Entity());
    ent->SetPosition(vec3((i * 2) - 15.0, 10 + i, 0));
    unique_ptr<Component> physComponent(new cPhysics());
    unique_ptr<Component> renderComponent(new cShapeRenderer());
    ent->AddComponent(unique_ptr<Component>(new cPhysics()));
    ent->AddComponent(unique_ptr<Component>(new cShapeRenderer()));
    SceneList.push_back(move(ent));
  }
  phys::SetCameraPos(vec3(20.0f, 10.0f, 20.0f));
  phys::SetCameraTarget(vec3(0, 10.0f, 0));
  InitPhysics();
  return true;
}

bool render() {
  for (auto &e : SceneList) {
    e->Render();
  }
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
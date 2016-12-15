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
#define physics_tick 1.0 / 600.0

static vector<unique_ptr<Entity>> SceneList;
static unique_ptr<Entity> floorEnt;

unique_ptr<Entity> CreateTrampoline(const dvec3 &pos) {
  // Oh my god! Tramapoline! Trambopoline!
  unique_ptr<Entity> ent(new Entity());
  unique_ptr<cPlaneCollider> pc(new cPlaneCollider());
  pc->normal_offset = normalize(dvec3(0, 8, 0) - pos);
  pc->position_offset = dvec3(0, 0.5, 0);
  pc->DebugDraw = true;
  ent->AddComponent(unique_ptr<Component>(move(pc)));
  ent->SetPosition(pos);
  return ent;
}

unique_ptr<Entity> CreateParticle(const dvec3 &pos) {
  unique_ptr<Entity> ent(new Entity());
  ent->SetPosition(pos);
  unique_ptr<Component> physComponent(new cParticle());
  unique_ptr<cShapeRenderer> renderComponent(new cShapeRenderer(cShapeRenderer::SPHERE));
  renderComponent->SetColour(phys::RandomColour());
  ent->AddComponent(physComponent);
  ent->AddComponent(unique_ptr<Component>(new cSphereCollider()));
  ent->AddComponent(unique_ptr<Component>(move(renderComponent)));
  return ent;
}

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
    if (length(e->GetPosition()) > 40.0) {
      e->getCompatibleComponent<cParticle>()->Teleport({0, 4, 0});
    }
    e->Update(delta_time);
  }

  phys::Update(delta_time);
  return true;
}

unique_ptr<Entity> ent2;
bool load_content() {
  phys::Init();
  const int count = 8;
  auto delta_angle = (2.0 * glm::pi<double>()) / static_cast<double>(count);
  for (size_t i = 0; i < count; i++) {
    auto pos = glm::dvec3(cos(i * delta_angle) * 16.0 / 2.0, 8.0 + i, -sin(i * delta_angle) * 16.0 / 2.0);
    SceneList.push_back(move(CreateParticle(pos)));
    pos.y = 2.0f;
    SceneList.push_back(CreateTrampoline(pos));
  }
  // invisible floor plane
  floorEnt = unique_ptr<Entity>(new Entity());
  {
    auto fc = unique_ptr<cPlaneCollider>(new cPlaneCollider());
    fc->infinite = true;
    floorEnt->AddComponent(unique_ptr<Component>(move(fc)));
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
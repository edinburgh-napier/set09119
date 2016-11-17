#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <graphics_framework.h>
#include <phys_utils.h>
#include <thread>

using namespace std;
using namespace graphics_framework;
using namespace glm;

#define physics_tick 1.0 / 60.0
#define fallheight 20.0

struct sBall {
  glm::dvec3 velocity;
  glm::dvec3 position;
};
static sBall ball;
static dvec3 gravity = dvec3(0, -10.0, 0);
static double truth_t = sqrt(abs(2.0 * gravity.y * fallheight)) / gravity.y;
static double truth_v = truth_t * gravity.y;
chrono::time_point<chrono::high_resolution_clock> tp_start;
chrono::time_point<chrono::high_resolution_clock> tp_end;

// use this function simulate render workload
void doWork() {
  // this_thread::sleep_for(std::chrono::milliseconds(rand()%50));
}

bool update(double delta_time) {
  doWork();
  static bool done = false;
  static uint16_t frames = 0;
  static uint16_t ticks = 0;
  static double t = 0.0;
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_SPACE)) {
    tp_start = chrono::high_resolution_clock::now();
    ball.position = vec3(0, fallheight, 0);
    ball.velocity = dvec3(0);
    done = false;
    ticks = 0;
    frames = 0;
    t = 0.0;
  }

  if (!done) {
    frames++;
    double remainingTime = delta_time;
    while (remainingTime > 0.0) {
      ticks++;
      double dt = glm::min(remainingTime, physics_tick);
      remainingTime -= dt;
      t += dt;

      // *********************************
      // Apply Accleration to Velocity

      // Apply Velocity to position

      // *********************************

      if (ball.position.y <= 0.0f) {
        tp_end = chrono::high_resolution_clock::now();
        const auto time = chrono::duration_cast<chrono::duration<double>>(tp_end - tp_start).count();
        cout << "\nFrames:" << frames << "\tTicks:" << ticks << "\tTicks Per Second : " << ticks / time
             << "\nFall Time: " << time << "s\tCollision Velocity" << ball.velocity.y << "m/s"
             << "\nTime Error:" << (abs(time) - truth_t) / truth_t
             << "%\tVelocity Error: " << (abs(ball.velocity.y) - truth_v) / truth_v << "%" << endl;
        ball.velocity.y = 0;
        done = true;
        break;
      }
    }
  }
  phys::Update(delta_time);
  return true;
}

bool load_content() {
  phys::Init();
  ball.velocity = dvec3(0);
  ball.position = dvec3(0);
  phys::SetCameraPos(vec3(20.0f, 10.0f, 20.0f));
  phys::SetCameraTarget(vec3(0, 10.0f, 0));
  tp_start = chrono::high_resolution_clock::now();
  tp_end = chrono::high_resolution_clock::now();
  cout << "\nPress Space to reset ball" << endl;
  renderer::toggle_vsync(false);
  return true;
}

bool render() {
  phys::DrawSphere(ball.position, 1.0f, RED);
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
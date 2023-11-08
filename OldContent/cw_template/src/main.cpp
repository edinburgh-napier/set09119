#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <graphics_framework.h>
#include <phys_utils.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

bool load_content() {
	phys::Init();
	return true;
}

bool update(float delta_time) {
	static float rot = 0.0f;
	rot += 0.2f * delta_time;
	phys::SetCameraPos(rotate(vec3(15.0f, 12.0f, 15.0f), rot, vec3(0, 1.0f, 0)));
	phys::Update(delta_time);
	return true;
}

bool render() {
	phys::DrawSphere(glm::vec3(4.0f, 4.0f, 0), 1.0f, RED);
	phys::DrawSphere(glm::vec3(-4.0f, 4.0f, 0), 1.0f, RED);
	phys::DrawSphere(glm::vec3(0, 8.0f, 0), 0.2f, YELLOW);
	phys::DrawSphere(glm::vec3(0), 1.0f, GREEN);
	phys::DrawSphere(glm::vec3(0, 4.0f, 4.0f), 1.0f, BLUE);
	phys::DrawSphere(glm::vec3(0, 4.0f, -4.0f), 1.0f, BLUE);
	phys::DrawCube(glm::vec3(0, 4.0f, 0));
	phys::DrawLine(glm::vec3(0, 4.0f, 4.0f), glm::vec3(0, 8.0f, 0));
	phys::DrawLine(glm::vec3(0, 4.0f, -4.0f), glm::vec3(0, 8.0f, 0));
	phys::DrawLine(glm::vec3(4.0f, 4.0f, 0), glm::vec3(0), true, ORANGE);
	phys::DrawLine(glm::vec3(-4.0f, 4.0f, 0), glm::vec3(0), true, PINK);
	phys::DrawLineCross(glm::vec3(0, 8.0f, 0), 1.0f, false);
	phys::DrawArrow(glm::vec3(0, 4.0f, 0), glm::vec3(0, 8.0f, 0), 1.0f, GREY);
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
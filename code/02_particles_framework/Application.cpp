#include <memory>
#include <functional>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/ext.hpp"

#include "Application.h"
#include "Camera.h"

// Some global variables to access from static function callbacks that GLFW uses
double lastX = 0;
double lastY = 0;
bool keys[1024];
bool firstMouse = true;
Camera camera;

// This data stores, since last polling time, which keys changes state from pressed to released and vice versa
struct key_state_t
{
	int keyCode = 0;
	bool pressed = true;
};
std::vector<key_state_t> latestKeyStateChanges;

// Moves/alters the camera positions based on user input
void Application::DoMovement(GLfloat deltaTime)
{
	glfwPollEvents();

	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

// Is called whenever a key is pressed/released via GLFW
static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		auto was_pressed = keys[key];
		if (action == GLFW_PRESS)
		{
			if (!was_pressed)
				latestKeyStateChanges.push_back({ key, true });
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			if (was_pressed)
				latestKeyStateChanges.push_back({ key, false });
			keys[key] = false;
		}
	}
}

static void mouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	double xOffset = xPos - lastX;
	double yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement((GLfloat)xOffset, (GLfloat)yOffset);
}


static void scrollCallback(GLFWwindow *window, double xOffset, double yOffset)
{
	camera.ProcessMouseScroll((GLfloat)yOffset);
}

// Renderer initialisation
int Application::InitWindow() {
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// static variables for callback function
	// window
	static const GLuint REQUESTED_WIDTH = 800;
	static const GLuint REQUESTED_HEIGHT = 600;

	m_window = glfwCreateWindow(REQUESTED_WIDTH, REQUESTED_HEIGHT, "Physics-Based Animation", nullptr, nullptr);

	if (nullptr == m_window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(m_window);
	glfwGetFramebufferSize(m_window, &m_width, &m_height);

	lastX = m_width / 2.0;
	lastY = m_height / 2.0;

	// Set the required callback functions
	glfwSetKeyCallback(m_window, keyCallback);
	glfwSetCursorPosCallback(m_window, mouseCallback);
	glfwSetScrollCallback(m_window, scrollCallback);

	// remove the mouse cursor
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// moder GLEW approach
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, m_width, m_height);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return 1;
}

// clear buffer
void Application::MainLoop() {

	// Initialise graphics
	InitWindow();

	// Initialise other things that depend on graphics
	meshDb.Init();
	shaderDb.Init();
	m_physEngine.Init(camera, meshDb, shaderDb);

	// Prepare some time bookkeeping
	const GLfloat timeStart = (GLfloat)glfwGetTime(); 
	GLfloat lastFrameTimeSinceStart = 0.0f;

	while (!glfwWindowShouldClose(m_window))
	{
		// time elapsed since application start
		GLfloat timeSinceStart = (GLfloat)glfwGetTime() - timeStart;

		// calculate the delta time since last frame
		auto deltaTime = timeSinceStart - lastFrameTimeSinceStart;

		// save the current time since start to the previous time since start, so that we can calculate the elapsed time between the different frames
		lastFrameTimeSinceStart = timeSinceStart;

		// poll input events
		glfwPollEvents();

		// Handle key state changes in the physics engine and clear them
		for (const auto& keyEvt : latestKeyStateChanges)
			m_physEngine.HandleInputKey(keyEvt.keyCode, keyEvt.pressed);
		latestKeyStateChanges.clear();

		// Update camera
		DoMovement(deltaTime);

		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Calculate view and projection matrices
		auto projection = glm::perspective(camera.GetZoom(), (GLfloat)m_width / (GLfloat)m_height, 0.1f, 1000.0f);
		auto view = camera.GetViewMatrix();

		// Update the physics
		m_physEngine.Update(deltaTime, timeSinceStart);

		// Draw all the objects in the physics engine
		m_physEngine.Display(view, projection);

		// Swap the buffers
		glfwSwapBuffers(m_window);
	}
	
	glfwTerminate();
}


int main(int argc, const char** argv)
{
	Application app;
	app.MainLoop();
	return 0;
}
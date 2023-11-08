#pragma once

#include <unordered_map>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include "Mesh.h"
#include "Shader.h"
#include "PhysicsEngine.h"

// A mesh database
class MeshDb
{
public:
	// Initialise the database with two meshes created in code
	void Init()
	{
		Add("tetra", Mesh(TetrahedronMeshData()));
		Add("plane", Mesh(PlaneMeshData()));
	}
	
	// Add a mesh to the database
	void Add(const std::string& name, Mesh& md)
	{
		data[name] = md;
	}
	
	// Fetch a pointer to a mesh given a name, null if not found
	const Mesh* Get(const std::string& name) const
	{
		auto it = data.find(name);
		return it != data.end() ? &it->second : nullptr;
	}

private:
	// Store the names->meshes as a hash map
	std::unordered_map<std::string, Mesh> data;
};

// A shader database
class ShaderDb
{
public:
	// Initialise the database with the default shader
	void Init()
	{
		Add("default", CreateDefaultShader());
	}

	// Add a shader to the database
	void Add(const std::string& name, Shader& md)
	{
		data[name] = Shader(md);
	}

	// Fetch a pointer to a shader given a name, null if not found
	const Shader* Get(const std::string& name) const
	{
		auto it = data.find(name);
		return it != data.end() ? &it->second : nullptr;
	}
private:
	// Store the names->shaders as a hash map
	std::unordered_map<std::string, Shader> data;
};

// The application class
class Application
{

public:
	// Execute the main loop. We never return from here
	void MainLoop();

private:

	// Initialise window-specific code, e.g. OpenGL 
	int InitWindow();

	// Moves the camera based on frame-to-frame time
	void DoMovement(GLfloat deltaTime);

private:

	// window handle and dimensions
	GLFWwindow* m_window = NULL;
	int m_width;
	int m_height;

	// the physics engine object
	PhysicsEngine m_physEngine;

	// the two databases
	MeshDb meshDb;
	ShaderDb shaderDb;
};


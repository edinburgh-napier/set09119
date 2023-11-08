#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include "Shader.h"

// Simple mesh data specification, used to initialise a mesh
struct SimpleMeshData
{
	std::vector<glm::vec3> vertices;
	std::vector<int> indices;
};

inline SimpleMeshData TetrahedronMeshData()
{
	SimpleMeshData smd;
	smd.vertices = {
		glm::vec3(0.0f,0.5f,0.0f), // top
		glm::vec3(-0.5f,-0.5f,0.5f), // front-left
		glm::vec3(0.5f,-0.5f,0.5f), // front-right
		glm::vec3(0.0f,-0.5f,-0.5f), // back
	};
	smd.indices = { 0,1,2, 0,2,3, 0,3,1, 1,3,2}; // 4 triangles
	return smd;
}

inline SimpleMeshData PlaneMeshData(const glm::vec2& halfExtents)
{
	SimpleMeshData smd;
	smd.vertices = {
		glm::vec3(-halfExtents.x,0,halfExtents.y),
		glm::vec3(halfExtents.x,0,halfExtents.y),
		glm::vec3(-halfExtents.x,0,-halfExtents.y),
		glm::vec3(halfExtents.x,0,-halfExtents.y)
	};
	smd.indices = { 0,1,3,3,2,0 }; // 2 triangles
	return smd;
}


// Mesh class, with OpenGL-specific data and transforms
class Mesh
{
public:

	

	// Initialise the mesh given some vertices
	void Init(const SimpleMeshData& meshData)
	{
		m_numIndices = GLsizei(meshData.indices.size());

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glGenBuffers(NUM_BUFFERS, m_buffers);

		// vertex data
		glBindBuffer(GL_ARRAY_BUFFER, m_buffers[POSITION_VB]);
		const int numComponents = 3; // A vertex has x,y,z coordinates
		auto bytesTotal = sizeof(glm::vec3) * meshData.vertices.size();
		glBufferData(GL_ARRAY_BUFFER, bytesTotal, &meshData.vertices[0].x, GL_STATIC_DRAW);
		glEnableVertexAttribArray(POSITION_VB);
		glVertexAttribPointer(POSITION_VB, numComponents, GL_FLOAT, GL_FALSE, 0, 0);

		// index data
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[IB]);
		bytesTotal = sizeof(int) * meshData.indices.size();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytesTotal, &meshData.indices[0], GL_STATIC_DRAW);

		glBindVertexArray(0);
	}

	// The actual draw call. We expect a shader to be bound and set-up accordingly
	void DrawVertexArray() const
	{
		glBindVertexArray(m_vao);
		glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, (void*)0);
		glBindVertexArray(0);
	}

private:
	enum {
		POSITION_VB=0, // position vertex buffer
		IB, // index buffer
		NUM_BUFFERS
	};

	GLuint m_vao=0; // vertex array object
	GLuint m_buffers[NUM_BUFFERS];
	GLsizei m_numIndices = 0;
	
};

// A generic renderable object, such as a particle or the ground, that we can render, move, rotate, etc
class Object
{
public:
	
	void Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const
	{
		m_shader->Use();
		m_shader->SetUniform("model", ModelMatrix());
		m_shader->SetUniform("view", viewMatrix);
		m_shader->SetUniform("projection", projectionMatrix);
		m_shader->SetUniform("color", m_color);
		m_mesh->DrawVertexArray();
	}

	// gets the position
	const glm::vec3& Position() const 
	{ 
		return m_position;  
	}

	// we must initialise it with a mesh and a shader
	void SetMesh(const Mesh* mesh) 
	{ 
		m_mesh = mesh; 
	}
	
	void SetShader(const Shader* shader) 
	{ 
		m_shader = shader; 
	}

	void SetColor(const glm::vec4& c) 
	{	
		m_color = c;	
	}

	void SetPosition(const glm::vec3& position) 
	{ 
		m_position = position; 
	}

	void SetScale(const glm::vec3& scale) 
	{ 
		m_scale = scale; 
	}

	// translate mesh by a vector
	void Translate(const glm::vec3& offset) 
	{ 
		m_position += offset; 
	}

	// rotate mesh by an axis,angle pair
	void Rotate(const float angleInRads, const glm::vec3& axis) 
	{ 
		m_orientation = glm::rotate(m_orientation, angleInRads, axis); 
	}

private:
	// getModel computes the model matrix any time it is required
	const glm::mat4 ModelMatrix() const
	{
		auto translateMatrix = glm::translate(glm::mat4(1.0f), m_position);
		auto scaleMatrix = glm::scale(glm::mat4(1.0f), m_scale);
		return translateMatrix * m_orientation * scaleMatrix;
	}

private:

	// A pointer to a const shader. Many particles can share the same shader, and we never modify it
	const Shader * m_shader = nullptr; 
	
	// A pointer to a const mesh. Many particles can share the same mesh, and we never modify it
	const Mesh * m_mesh = nullptr;

	// color
	glm::vec4 m_color = glm::vec4(0.5f,0.5f,0.5f,1.0f);

	// Transformation data
	glm::vec3 m_position = glm::vec3(0.0f);
	glm::vec3 m_scale = glm::vec3(1.0f);
	glm::mat4 m_orientation = glm::mat4(1.0f);
};



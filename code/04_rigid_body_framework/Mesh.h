#pragma once

#include <unordered_map>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include "OBJloader.h"

// Simple mesh data specification, used to initialise a mesh
struct MeshDataStream
{
	// Each vertex has 3 components: x,y,z (for positions and normals)
	std::vector<glm::vec3> data;
	// Each triangle face has 3 indices 
	std::vector<glm::ivec3> faces;
};

struct MeshData
{
	MeshDataStream positions;
	MeshDataStream normals;
};

MeshData MeshDataFromWavefrontObj(const char* filename);
MeshData TetrahedronMeshData();
MeshData PlaneMeshData();


// Mesh class, with OpenGL-specific data
class Mesh
{
public:

	// Uninitialised mesh
	Mesh() {}

	// Mesh initialised with data
	Mesh(const MeshData& meshData) { Init(meshData);  }

	// Initialise the mesh given some vertices
	void Init(const MeshData& meshData);

	// The actual draw call. We expect a shader to be bound and set-up accordingly
	void DrawVertexArray() const;

	const MeshData& Data() const { return m_meshData; }

private:
	enum {
		POSITION_VB=0, // position vertex buffer
		NORMAL_VB, // normal vertex buffer
		IB, // index buffer
		NUM_BUFFERS
	};

	GLuint m_vao=0; // vertex array object
	GLuint m_buffers[NUM_BUFFERS] = {0,0}; // Buffer IDs. Initialise it to zeroes (==invalid buffer IDs)

	MeshData m_meshData;
};


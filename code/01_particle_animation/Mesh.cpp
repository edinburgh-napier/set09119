#include "Mesh.h"
#include <errno.h>

#include "Path.h"

/*
**	MESH 
*/

// default constructor creates a horizontal plane or dimensions 1 x 1 centered on the origin
Mesh::Mesh()
{
	// Create vertices
	Vertex vertices[] = { 
		Vertex(glm::vec3(-0.5f ,0.0f ,-0.5f)),
		Vertex(glm::vec3(0.5f, 0.0f, -0.5f)),
		Vertex(glm::vec3(-0.5f ,0.0f ,0.5f)),
		Vertex(glm::vec3(0.5f, 0.0f, -0.5f)),
		Vertex(glm::vec3(-0.5f ,0.0f ,0.5f)),
		Vertex(glm::vec3(0.5f ,0.0f ,0.5f))
	};

	//create mesh
	initMesh(vertices, sizeof(vertices) / sizeof(vertices[0]));

	// initialise tansform matrices (identify)
	initTransform();

	// Setup and compile shaders
	m_shader=Shader(ResourcesPath() + "shaders/core.vert", ResourcesPath() + "shaders/core.frag");
	m_shader.Use();
}

// create mesh from a .obj file
Mesh::Mesh(const std::string& fileName)
{
	InitMesh(OBJModel(fileName).ToIndexedModel());
	initTransform();

	// Setup and compile shaders
	m_shader = Shader(ResourcesPath() + "shaders/core.vert", ResourcesPath() + "shaders/core.frag");
	m_shader.Use();
}

Mesh::~Mesh()
{
}


/* 
** INIT METHODS 
*/

// initialise transform matrices to identity
void Mesh::initTransform() {
	m_translate = glm::mat4(1.0f);
	m_rotate = glm::mat4(1.0f);
	m_scale = glm::mat4(1.0f);
}

// create mesh from vertices
void Mesh::initMesh(Vertex* vertices, unsigned int numVertices) {
	m_numIndices = numVertices;

	glGenVertexArrays(1, &m_vertexArrayObject);
	glBindVertexArray(m_vertexArrayObject);

	glGenBuffers(NUM_BUFFERS, m_vertexArrayBuffers);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[POSITION_VB]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(vertices[0]), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
}


// create mesh from model (typically loaded from file)
void Mesh::InitMesh(const IndexedModel& model)
{
	m_numIndices = model.indices.size();

	glGenVertexArrays(1, &m_vertexArrayObject);
	glBindVertexArray(m_vertexArrayObject);

	glGenBuffers(NUM_BUFFERS, m_vertexArrayBuffers);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[POSITION_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(model.positions[0]) * model.positions.size(), &model.positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(model.texCoords[0]) * model.texCoords.size(), &model.texCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(model.normals[0]) * model.normals.size(), &model.normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexArrayBuffers[INDEX_VB]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(model.indices[0]) * model.indices.size(), &model.indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

// load .obj file
void Mesh::loadOBJ(const char * path,
	std::vector < glm::vec3 > & out_vertices,
	std::vector < glm::vec2 > & out_uvs,
	std::vector < glm::vec3 > & out_normals
) {
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;

	FILE * stream;
	errno_t err;

	err = fopen_s(&stream, path, "r");

	if (err != 0)
	{
		printf("The file was not opened\n");
	}
	else{
		while (1) {

			char lineHeader[128];
			// read the first word of the line
			int res = fscanf(stream, "%s", lineHeader);
			if (res == EOF)
				break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader
			if (strcmp(lineHeader, "v") == 0) {
				glm::vec3 vertex;
				fscanf_s(stream, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				temp_vertices.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) {
				glm::vec2 uv;
				fscanf_s(stream, "%f %f\n", &uv.x, &uv.y);
				temp_uvs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0) {
				glm::vec3 normal;
				fscanf_s(stream, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				temp_normals.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) {
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf_s(stream, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9) {
					std::cerr << "file can't be read by parser" << std::endl;
				}
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
		}
	}
	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		out_vertices.push_back(vertex);
	}

}



/*
** TRANSFORMATION METHODS
*/

// translate
void Mesh::translate(const glm::vec3 &vect) {
	m_translate = glm::translate(m_translate, vect);
}

// rotate
void Mesh::rotate(const float &angle, const glm::vec3 &vect) {
	m_rotate = glm::rotate(m_rotate, angle, vect);
}

// scale
void Mesh::scale(const glm::vec3 &vect) {
	m_scale = glm::scale(m_scale, vect);
}



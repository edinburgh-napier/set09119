#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

MeshData TetrahedronMeshData()
{
	MeshData smd;
	smd.positions.data = {
		glm::vec3(0.0f,0.5f,0.0f), // top
		glm::vec3(-0.5f,-0.5f,0.5f), // front-left
		glm::vec3(0.5f,-0.5f,0.5f), // front-right
		glm::vec3(0.0f,-0.5f,-0.5f), // back
	};
	smd.positions.faces = { {0,1,2}, {0,2,3}, {0,3,1}, {1,3,2} }; // 4 triangles
	return smd;
}

MeshData PlaneMeshData()
{
	glm::vec2 halfExtents = glm::vec2(0.5f);
	MeshData smd;
	smd.positions.data = {
		glm::vec3(-halfExtents.x,0,halfExtents.y),
		glm::vec3(halfExtents.x,0,halfExtents.y),
		glm::vec3(-halfExtents.x,0,-halfExtents.y),
		glm::vec3(halfExtents.x,0,-halfExtents.y)
	};
	smd.positions.faces = { {0,1,3}, {3,2,0} }; // 2 triangles
	return smd;
}

std::vector<std::string> split(const std::string& text, char delimiter)
{
	std::vector<std::string> tokens;

	size_t pos = 0, offset=0;
	std::string token;
	while ((pos = text.find(delimiter, offset)) != std::string::npos) {
		tokens.push_back( text.substr(offset, pos-offset));
		offset = pos + 1;
	}
	if(offset != text.size())
		tokens.push_back(text.substr(offset, text.size() - offset));
	return tokens;
}

MeshData MeshDataFromWavefrontObj(const char * filename)
{
	MeshData smd;
	using namespace tinyobj;
	attrib_t attrib;
	std::vector<shape_t> shapes;
	std::vector<material_t> materials;
	std::string err;

	auto ok = LoadObj(&attrib, &shapes, &materials, &err, filename);
	if (!ok)
		printf("TinyObjLoader ERROR for file %s: %s\n", filename, err.c_str());
	else
	{
		smd.positions.data.resize(attrib.vertices.size() / 3);
		for (size_t i = 0; i < smd.positions.data.size(); ++i)
			smd.positions.data[i] = glm::vec3(attrib.vertices[i * 3], attrib.vertices[i * 3 + 1], attrib.vertices[i * 3 + 2]);
		smd.normals.data.resize(attrib.normals.size() / 3);
		for (size_t i = 0; i < smd.normals.data.size(); ++i)
			smd.normals.data[i] = glm::vec3(attrib.normals[i * 3], attrib.normals[i * 3 + 1], attrib.normals[i * 3 + 2]);
		for (const auto& shape : shapes)
		{
			const auto& indices = shape.mesh.indices;
			for ( size_t mi = 0; mi < indices.size(); mi += 3)
			{
				smd.positions.faces.emplace_back(indices[mi].vertex_index, indices[mi + 1].vertex_index, indices[mi + 2].vertex_index);
				if(!smd.normals.data.empty())
					smd.normals.faces.emplace_back(indices[mi].normal_index, indices[mi + 1].normal_index, indices[mi + 2].normal_index);
			}
		}
	}
	return smd;
}

// Prepares the mesh for visualisation:
//	Create normals if we don't have any
//	Replicate vertices/normals so that we have 1:1 correspondence
MeshData PrepareMesh(const MeshData& md_in)
{
	auto md_out = md_in;
	if (md_out.normals.data.empty())
	{
		md_out.normals.faces = md_out.positions.faces; // copy face indexing
		const auto& v = md_out.positions.data; // alias
		md_out.normals.data.resize(v.size(), glm::vec3(0.0f));
		for(const auto& face : md_out.positions.faces)
		{
			auto n = normalize(cross(v[face.y] - v[face.x], v[face.z] - v[face.x]));
			md_out.normals.data[face.x] += n;
			md_out.normals.data[face.y] += n;
			md_out.normals.data[face.z] += n;
		}
		for (auto& normal : md_out.normals.data)
			normal = normalize(normal);
	}
	else if (md_out.normals.data.size() != md_out.positions.data.size()) // if normals/vertices don't match in size, duplicate as needed so that we have 1:1 correspondence
	{
		auto numFaces = md_in.positions.faces.size();
		md_out.positions.data.resize(numFaces * 3);
		md_out.normals.data.resize(numFaces * 3);
		for (int i = 0; i < int(md_in.positions.faces.size()); ++i)
		{
			glm::ivec3 face;
			for (int j = 0; j < 3; ++j)
			{
				auto o = i * 3 + j;
				md_out.positions.data[o] = md_in.positions.data[md_in.positions.faces[i][j]];
				md_out.normals.data[o] = md_in.normals.data[md_in.normals.faces[i][j]];
				face[j] = o;
			}
			md_out.positions.faces[i] = face;
			md_out.normals.faces[i] = face;
		}
	}
	return md_out;
}

void Mesh::Init(const MeshData& meshData_in)
{
	m_meshData = meshData_in; // Store the original
	auto meshData = PrepareMesh(meshData_in); // convert for ease of visualisation

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(NUM_BUFFERS, m_buffers);

	// vertex data
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[POSITION_VB]);
	const int numComponents = 3; // A vertex has x,y,z coordinates
	auto bytesTotal = sizeof (glm::vec3) * meshData.positions.data.size();
	glBufferData(GL_ARRAY_BUFFER, bytesTotal, &meshData.positions.data[0].x, GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION_VB);
	glVertexAttribPointer(POSITION_VB, numComponents, GL_FLOAT, GL_FALSE, 0, 0);

	// normal data
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, bytesTotal, &meshData.normals.data[0].x, GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMAL_VB);
	glVertexAttribPointer(NORMAL_VB, numComponents, GL_FLOAT, GL_FALSE, 0, 0);

	// index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[IB]);
	bytesTotal = sizeof(glm::ivec3) * meshData.positions.faces.size();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytesTotal, &meshData.positions.faces[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

// The actual draw call. We expect a shader to be bound and set-up accordingly
void Mesh::DrawVertexArray() const
{
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, GLsizei(m_meshData.positions.faces.size()*3), GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);
}
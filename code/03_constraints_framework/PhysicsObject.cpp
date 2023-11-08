#include "PhysicsObject.h"

#include <glm/glm.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Force.h"

void PhysicsBody::Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const
{
	m_shader->Use();
	//m_shader->SetUniform("model", ModelMatrix());
	//m_shader->SetUniform("view", viewMatrix);
	//m_shader->SetUniform("projection", projectionMatrix);
	m_shader->SetUniform("color", m_color);

	auto mvp = projectionMatrix * viewMatrix * ModelMatrix();
	m_shader->SetUniform("modelViewProjectionMatrix", mvp);
	m_shader->SetUniform("normalMatrix", transpose(inverse(viewMatrix * ModelMatrix())));
	m_mesh->DrawVertexArray();
}
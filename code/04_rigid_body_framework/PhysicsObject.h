#pragma once

#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader;
class Mesh;
class Force;

// A generic renderable object, such as a particle or the ground, that we can render, move, rotate, etc
class PhysicsBody
{
public:

	// If we're going to derive from this class, create a virtual destructor that does nothing
	virtual ~PhysicsBody() {}

	void Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const;

	// gets the position
	const glm::vec3& Position() const
	{
		return m_position;
	}

	const glm::vec3& Scale() const
	{
		return m_scale;
	}

	const glm::mat4& Orientation() const
	{
		return m_orientation;
	}

	const Mesh * GetMesh() const
	{ 
		return m_mesh; 
	}

	// we must initialise it with a mesh and a shader
	void SetMesh(const Mesh * mesh)
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

	void SetOrientation(const glm::mat4& m) {
		m_orientation = m;
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

	// getModel computes the model matrix any time it is required
	const glm::mat4 ModelMatrix() const
	{
		auto translateMatrix = glm::translate(glm::mat4(1.0f), m_position);
		auto scaleMatrix = glm::scale(glm::mat4(1.0f), m_scale);
		return translateMatrix * m_orientation * scaleMatrix;
	}

private:

	// A pointer to a const shader. Many particles can share the same shader, and we never modify it
	const Shader* m_shader = nullptr;

	// A pointer to a const mesh. Many particles can share the same mesh, and we never modify it
	const Mesh* m_mesh = nullptr;

	// color
	glm::vec4 m_color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

	// Transformation data
	glm::vec3 m_position = glm::vec3(0.0f);
	glm::vec3 m_scale = glm::vec3(1.0f);
	glm::mat4 m_orientation = glm::mat4(1.0f);
};

// A particle is a physics body without shape/size. 
class Particle : public PhysicsBody
{
public:

	void SetCoefficientOfRestitution(float cor) { m_cor = cor; }
	void SetMass(float mass) { m_mass = mass; }
	void SetVelocity(const glm::vec3& velocity) { m_velocity = velocity; }
	
	// Call this at the beginning of a simulation step
	void ClearForcesImpulses() { m_accumulatedForce = glm::vec3(0.0f);  m_accumulatedImpulse = glm::vec3(0.0f); }
	// Adds to the sum of forces
	void ApplyForce(const glm::vec3& force) { m_accumulatedForce += force; }
	// Adds to the sum of impulses
	void ApplyImpulse(const glm::vec3& impulse) { m_accumulatedImpulse += impulse; }
	
	
	float Mass() const { return m_mass; }
	const glm::vec3& Velocity() const { return m_velocity; }
	const glm::vec3& AccumulatedForce() { return m_accumulatedForce; }
	const glm::vec3& AccumulatedImpulse() { return m_accumulatedImpulse; }
	float CoefficientOfRestitution() { return m_cor; }

private:
	float m_cor = 0.9f;									// Coefficient of restitution
	float m_mass = 1.0f;								// Particle mass, in kg
	glm::vec3 m_velocity = glm::vec3(0.0f);				// Velocity, in m/s. Important! Must initialise (like this here), otherwise starting value would be undefined
	glm::vec3 m_accumulatedForce = glm::vec3(0.0f);		// Accumulated force in a single simulation step
	glm::vec3 m_accumulatedImpulse = glm::vec3(0.0f);	// Accumulated impulse in a single simulation step
};

class RigidBody : public Particle
{
public:
	
	void SetAngularVelocity(const glm::vec3& angVel) { m_angularVelocity = angVel; }
	//void SetAngularAcceleration(const glm::vec3& angAccel) { m_angularAcceleration = angAccel; }

	const glm::vec3& AngularVelocity() const { return m_angularVelocity; }
	//const glm::vec3& AngularAcceleration() const { return m_angularAcceleration; }
	glm::mat3 InverseInertia();
private:
	glm::vec3 m_angularVelocity = glm::vec3(0.0f);
	//glm::vec3 m_angularAcceleration = glm::vec3(0.0f);
};
#include "Rigid.h"

glm::dvec3 Rigid::s_gravity = { 0, -9.8, 0 };

Rigid::Rigid(const glm::dmat4& modelMat)
{
	// Puntero a la posici�n de la entidad
	double* ptr = (double*)&modelMat;
	m_position = (glm::dvec3*) &ptr[12];

	// Valores por defecto
	m_useGravity = true;
	m_velocity = { 0, 0, 0 };
	m_acceleration = { 0, 0 ,0 };
	m_torque = { 0, 0, 0 };
	m_mass = 1.0;
	m_drag = 0.0;
	m_angularDrag = 0.0;

	m_collider = nullptr;
}

void Rigid::update(GLuint deltaTime)
{
	// Actualizar aceleraci�n en funci�n de las fuerzas existentes
	if (m_useGravity)
		m_acceleration += s_gravity;

	// Actualizar velocidad en funci�n de la aceleraci�n
	m_velocity += m_acceleration * (deltaTime / 1000.0);

	// Actualizar la posici�n en funci�n de la velocidad
	*m_position += m_velocity * (deltaTime / 1000.0);

	// Rozamiento
	m_velocity *= (1 - (m_drag * deltaTime / 1000.0));

	// Limpiar la aceleraci�n
	m_acceleration = { 0, 0, 0 };

	// Aplicar el torque acumulado
	entity->rotate(glm::length(m_torque) * (deltaTime / 1000.0), glm::normalize(m_torque));

	// Rozamiento angular
	m_torque *= (1 - (m_angularDrag * deltaTime / 1000.0));
}

void Rigid::addForce(const glm::vec3& force)
{
	m_acceleration += force / m_mass;
}

void Rigid::addTorque(const glm::vec3& torque)
{
	m_torque += torque;
}
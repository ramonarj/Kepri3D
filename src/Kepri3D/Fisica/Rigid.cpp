#include "Rigid.h"

glm::dvec3 Rigid::s_gravity = { 0, -9.8, 0 };

Rigid::Rigid(const glm::dmat4& modelMat)
{
	// Puntero a la posición de la entidad
	double* ptr = (double*)&modelMat;
	m_position = (glm::dvec3*) &ptr[12];

	// Valores por defecto
	m_useGravity = true;
	m_velocity = { 0, 0, 0 };
	m_acceleration = { 0, 0 ,0 };
	m_mass = 1.0;
	m_drag = 0.0;

	m_collider = nullptr;
}

void Rigid::update(GLuint deltaTime)
{
	// Actualizar aceleración en función de las fuerzas existentes
	if (m_useGravity)
		m_acceleration += s_gravity;

	// Actualizar velocidad en función de la aceleración
	m_velocity += m_acceleration * (deltaTime / 1000.0);
	// Capar la velocidad máxima
	//if(glm::length(m_velocity) > 20.0)
	//{
	//	//std::cout << "Velocidad máxima" << std::endl;
	//	m_velocity = glm::normalize(m_velocity) * 20.0;
	//}

	// Actualizar la posición en función de la velocidad
	*m_position += m_velocity * (deltaTime / 1000.0);

	// Limpiar la aceleración
	m_acceleration = { 0, 0, 0 };
}

void Rigid::addForce(const glm::vec3& force)
{
	m_acceleration += force / m_mass;
}
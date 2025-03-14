#include "Rigid.h"

glm::dvec3 Rigid::s_gravity = { 0, -9.8, 0 };

Rigid::Rigid(const glm::dmat4& modelMat)
{
	// Puntero a la posición de la entidad
	double* ptr = (double*)&modelMat;
	m_position = (glm::dvec3*) &ptr[12];

	// Valores por defecto
	m_type = Dynamic;
	m_useGravity = true;
	m_mass = 1.0;

	m_velocity = { 0, 0, 0 };
	m_acceleration = { 0, 0 ,0 };
	m_drag = 0.0;

	m_angularVel = { 0, 0, 0 };
	m_angularAcc = { 0, 0 ,0 };
	m_angularDrag = 0.0;

	m_collider = nullptr;
}

void Rigid::update(GLuint deltaTime)
{
	// Actualizar aceleración en función de las fuerzas existentes
	if (m_type == RigidType::Dynamic && m_useGravity)
		m_acceleration += s_gravity;

	// - - LINEAL - - //
	// Actualizar velocidad en función de la aceleración
	m_velocity += m_acceleration * (deltaTime / 1000.0);

	// Actualizar la posición en función de la velocidad
	*m_position += m_velocity * (deltaTime / 1000.0);

	// Rozamiento
	m_velocity *= (1 - (m_drag * deltaTime / 1000.0));

	// Limpiar la aceleración
	m_acceleration = { 0, 0, 0 };

	// - - ANGULAR - - /
	// Actualizar velocidad angular en función de la aceleración angular
	m_angularVel += m_angularAcc * (deltaTime / 1000.0);

	// Actualizar la rotación en función de la velocidad angular
	entity->rotate(glm::length(m_angularVel) * (deltaTime / 1000.0), glm::normalize(m_angularVel));

	// Rozamiento angular
	m_angularVel *= (1 - (m_angularDrag * deltaTime / 1000.0));

	// Limpiar la aceleración angular
	m_angularAcc = { 0, 0, 0 };
}

void Rigid::addForce(const glm::vec3& force)
{
	m_acceleration += force / m_mass;
}

void Rigid::addTorque(const glm::vec3& torque)
{
	m_angularAcc += torque / m_mass;
}

void Rigid::addForce(const glm::vec3& force, const glm::vec3& point)
{
	if (glm::length(point) == 0) { addForce(force); return; }

	// Ángulo entre el vector posición y el vector fuerza
	double cosAngle = glm::dot(glm::normalize(force), glm::normalize(point));
	
	// Aplicar el torque necesario usando la componente tangencial de la fuerza. L = r x F
	addTorque(glm::cross(point, force));

	// Por descomposición de fuerzas, una vez quitada la tangencial (que va al torque),
	// nos queda la que apunta desde el punto al centro de masas
	float fuerzaNoTang = glm::length(force) * cosAngle;

	// Si angle > 90 está "tirando", si es menor, está "empujando"
	addForce(fuerzaNoTang * point);
}
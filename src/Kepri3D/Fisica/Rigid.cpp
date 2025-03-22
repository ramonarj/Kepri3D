#include "Rigid.h"

#include "Game.h"
#include "PhysicsSystem.h"

// Si el Rigid está 20 frames con una velocidad ínfima, se duerme
const unsigned int FRAMES_UNTIL_SLEEP = 20;

Rigid::Rigid(const glm::dmat4& modelMat, RigidType type)
{
	// Puntero a la posición de la entidad
	double* ptr = (double*)&modelMat;
	m_position = (glm::dvec3*) &ptr[12];

	// Valores por defecto
	m_type = type;
	m_useGravity = true;
	m_mass = 1.0;

	m_velocity = { 0, 0, 0 };
	m_acceleration = { 0, 0 ,0 };
	m_drag = 0.2;
	m_accumForces = { 0, 0, 0 };

	m_angularVel = { 0, 0, 0 };
	m_angularAcc = { 0, 0 ,0 };
	m_angularDrag = 0.0;
	m_accumTorque = { 0, 0, 0 };

	m_collider = nullptr;
	m_sleeping = (m_type == Dynamic) ? false : true;
	m_framesInactivo = 0;
}

void Rigid::update(GLuint delta)
{
	if (m_type == RigidType::Static || m_sleeping) { return; }
	real deltaTime = delta / 1000.0; // Pasar de ms a segundos

	// Darle una aceleración fija si le afecta la gravedad
	if (m_useGravity)
		m_acceleration += PhysicsSystem::s_gravity;

	// - - LINEAL - - //
	// Actualizar aceleración en función de las fuerzas acumuladas (a = F / m)
	m_acceleration += m_accumForces / m_mass;

	// Actualizar velocidad en función de la aceleración (v = v0 + a*t)
	m_velocity += m_acceleration * deltaTime;

	// Actualizar la posición en función de la velocidad (e = e0 + v*t)
	*m_position += m_velocity * deltaTime;

	// Rozamiento
	m_velocity *= (1 - (m_drag * deltaTime));

	// Limpiar la aceleración y las fuerzas acumuladas
	m_acceleration = { 0, 0, 0 };
	m_accumForces = { 0, 0, 0 };

	// - - ANGULAR - - //
	// Actualizar aceleración angular en función del torque acumulado
	m_angularAcc += m_accumTorque / m_mass;

	// Actualizar velocidad angular en función de la aceleración angular
	m_angularVel += m_angularAcc * deltaTime;

	// Actualizar la rotación en función de la velocidad angular
	entity->rotate(glm::length(m_angularVel) * deltaTime, glm::normalize(m_angularVel));

	// Rozamiento angular
	m_angularVel *= (1 - (m_angularDrag * deltaTime));

	// Limpiar la aceleración angular y el torque acumulado
	m_angularAcc = { 0, 0, 0 };
	m_accumTorque = { 0, 0 ,0 };

	// - - Otros - - //
	// Ponerlo a dormir si es necesario
	if(glm::length(m_velocity) < 0.1 && glm::length(m_angularVel) < 0.1)
	{
		m_framesInactivo++;
		if (m_framesInactivo >= FRAMES_UNTIL_SLEEP)
			sleep();
	}
	// Reiniciar la cuenta
	else { m_framesInactivo = 0; }
}

void Rigid::addForce(const vector3& force)
{
	if (m_type == Static || glm::length(force) < 0.05) { return; }

	m_accumForces += force;
	wakeUp();
}

void Rigid::addTorque(const vector3& torque)
{
	if (m_type == Static || glm::length(torque) < 0.05) { return; }

	m_accumTorque += torque;
	wakeUp();
}

void Rigid::addForce(const vector3& force, const vector3& point)
{
	if (glm::length(point) == 0) { addForce(force); return; }

	// Ángulo entre el vector posición y el vector fuerza
	real cosAngle = glm::dot(glm::normalize(force), glm::normalize(point));
	
	// Aplicar el torque necesario usando la componente tangencial de la fuerza. L = r x F
	addTorque(glm::cross(point, force));

	// Por descomposición de fuerzas, una vez quitada la tangencial (que va al torque),
	// nos queda la que apunta desde el punto al centro de masas
	real fuerzaNoTang = glm::length(force) * cosAngle;

	// Si angle > 90 está "tirando", si es menor, está "empujando"
	addForce(fuerzaNoTang * point);
}

void Rigid::wakeUp()
{
	if (!m_sleeping || m_type == Static) { return; }

	m_sleeping = false;
	m_framesInactivo = 0;
#ifdef __DEBUG_INFO__
	PhysicsSystem::Instance()->rigidsDespiertos++;
#endif
}

void Rigid::sleep()
{
	if (m_sleeping) { return; }

	m_sleeping = true;
	m_velocity = { 0, 0, 0 };
#ifdef __DEBUG_INFO__
	PhysicsSystem::Instance()->rigidsDespiertos--;
#endif
}

void Rigid::setVelocity(const vector3& vel)
{
	if (m_type == Static) { return; }

	m_velocity = vel;
	// Despertarlo si es preciso
	if (m_sleeping && glm::length(m_velocity) > 0.05)
		wakeUp();
}
#include "PhysicsSystem.h"

#include "Rigid.h"
#include "Muelle.h"
#include "Collider.h"
#include <iostream>

PhysicsSystem* PhysicsSystem::s_instance = nullptr;

void PhysicsSystem::addRigid(Rigid* r)
{
	Collider* col = r->getEntity()->getComponent<Collider>();
	if (col == nullptr)
		std::cout << "La entidad " << r->getEntity()->getName() << " necesita un Collider" << std::endl;
	r->setCollider(col);

	m_rigids.push_back(r);
}

void PhysicsSystem::addMuelle(Muelle* m)
{
	m_muelles.push_back(m);
}

void PhysicsSystem::update(GLuint deltaTime)
{
	m_deltaTime = deltaTime;

	// Actualizar muelles
	for (Muelle* m : m_muelles)
	{
		double elongacion = glm::length(*m->r2->m_position - *m->r1->m_position) - m->longitud;
		glm::dvec3 R1toR2 = glm::normalize(*m->r2->m_position - *m->r1->m_position);
		if (elongacion == 0) { continue; }
		// Ley de Hooke
		if(m->r1->m_type != Static)
			m->r1->addForce(m->k * (R1toR2 * elongacion));
		if (m->r2->m_type != Static)
			m->r2->addForce(m->k * (-R1toR2 * elongacion));
	}
#ifdef __DEBUG_INFO__
	momentoTotal = { 0, 0 ,0 };
#endif
	// Comprobar y resolver colisiones entre Rigids
	for(int i = 0; i < m_rigids.size(); i++)
	{
		Rigid* r1 = m_rigids[i];
#ifdef __DEBUG_INFO__
		momentoTotal += ((glm::vec3)r1->m_velocity * r1->m_mass);
#endif
		for(int j = i + 1; j < m_rigids.size(); j++)
		{
			Rigid* r2 = m_rigids[j];
			// Hay colisión
			if(checkCollision(r1->m_collider, r2->m_collider))
			{
				//std::cout << "Colisión entre " << r1->getEntity()->getName() << " y " << r2->getEntity()->getName() << std::endl;
				// La resolvemos
				solveCollision(r1, r2);

				// Notificamos a los involucrados
				notifyCollision(r1->m_collider, r2->m_collider);
			}
		}
	}
}

bool PhysicsSystem::checkCollision(Collider* c1, Collider* c2)
{
	float dist = glm::length(c1->getEntity()->getPosition() - c2->getEntity()->getPosition());

	// Colisión Esfera-Esfera
	if(c1->shape == Collider::Esfera && c2->shape == Collider::Esfera)
	{
		if (dist < (c1->radio + c2->radio))
			return true;	
	}
	// Colisión Cubo-Cubo (AABB)
	else if(c1->shape == Collider::Cubo && c2->shape == Collider::Cubo)
	{
		glm::dvec3 halfExt1 = c1->halfExtents;
		glm::dvec3 halfExt2 = c2->halfExtents;
		glm::dvec3 posC1 = c1->getEntity()->getPosition();
		glm::dvec3 posC2 = c2->getEntity()->getPosition();
		
		if(posC1.x + halfExt1.x > posC2.x - halfExt2.x && posC1.x - halfExt1.x < posC2.x + halfExt2.x && // Eje X
			posC1.y + halfExt1.y > posC2.y - halfExt2.y && posC1.y - halfExt1.y < posC2.y + halfExt2.y && // Eje Y
			posC1.z + halfExt1.z > posC2.z - halfExt2.z && posC1.z - halfExt1.z < posC2.z + halfExt2.z) // Eje Z
			return true;
	}
	return false;
}

void PhysicsSystem::solveCollision(Rigid* r1, Rigid* r2)
{
	// - - - - (1) Hacer que dejen de solaparse (tirando hacia afuera) - - - - //
	unsigned int iter = 0;
	do
	{
		// TODO: dejar de detectar colisiones cuando ambos objetos ya están quietos
		r1->getEntity()->translate(-r1->m_velocity * (m_deltaTime / 1000.0));
		r2->getEntity()->translate(-r2->m_velocity * (m_deltaTime / 1000.0));
		iter++;
	} while (iter < MAX_ITER && checkCollision(r1->m_collider, r2->m_collider));

	// - - - - (2) Calcular el punto de contacto (según el tipo de los colliders) - - - - //
	glm::vec3 R1toR2;
	// Esfera-Esfera
	if (r1->m_collider->shape == Collider::Esfera && r2->m_collider->shape == Collider::Esfera)
	{
		R1toR2 = glm::normalize(r2->getEntity()->getPosition() - r1->getEntity()->getPosition());
	}
	// Cubo - Cubo
	else if (r1->m_collider->shape == Collider::Cubo && r2->m_collider->shape == Collider::Cubo)
	{
		R1toR2 = r2->getEntity()->getPosition() - r1->getEntity()->getPosition();
		glm::dvec3 absV = glm::abs((glm::dvec3)R1toR2 / r1->m_collider->halfExtents); //inspiración divina
		// No se me ocurre una forma mejor
		glm::vec3 eje;
		if (absV.x > absV.y && absV.x > absV.z) { eje = { 1, 0, 0 }; }
		else if (absV.y > absV.x && absV.y > absV.z) { eje = { 0, 1, 0 }; }
		else { eje = { 0, 0, 1 }; }
		R1toR2 *= eje;
		R1toR2 = glm::normalize(R1toR2);
	}

	// - - - - (3) Aplicar las nuevas velocidades - - - - //
	// Ambos dinámicos
	if (r1->m_type == Dynamic && r2->m_type == Dynamic)
	{
		// prueba
		//std::pair<glm::vec3, glm::vec3> vels = calculateElasticCollision(r1->m_velocity, r2->m_velocity,
		//	R1toR2, r1->m_mass, r2->m_mass);
		//r1->m_velocity = vels.first;
		//r2->m_velocity = vels.second;
		
		// Intercambiar velocidades y multiplicar por coef.rest.
		glm::vec3 auxVel = r1->m_velocity;
		r1->m_velocity = -R1toR2 * (float)glm::length(r2->m_velocity) * COEF_RESTITUCION;
		r2->m_velocity = R1toR2 * (float)glm::length(auxVel) * COEF_RESTITUCION;
	}
	// Uno estático; se quedan con su velocidad
	else if (r1->m_type == Static)
		r2->m_velocity = R1toR2 * (float)glm::length(r2->m_velocity) * COEF_RESTITUCION;
	else if (r2->m_type == Static)
		r1->m_velocity = -R1toR2 * (float)glm::length(r1->m_velocity) * COEF_RESTITUCION;

	// TODO conservación del momento lineal
}

void PhysicsSystem::notifyCollision(Collider* c1, Collider* c2)
{
	// Notificamos a las entidades involucradas (a cada componente)
	for (Component* c : c1->getEntity()->getComponents())
		c->onCollision(c2);
	for (Component* c : c2->getEntity()->getComponents())
		c->onCollision(c1);
}

std::pair<glm::vec3, glm::vec3> PhysicsSystem::calculateElasticCollision(const glm::dvec3& v1, const glm::dvec3& v2,
	const glm::dvec3& x1, const glm::dvec3& x2, double m1, double m2)
{
	double massTerm = (2.0 * m2 / (m1 + m2));
	glm::dvec3 v1new = v1 + massTerm * (glm::dot(v2 - v1, x2 - x1) / pow(glm::length(x2 - x1), 2))
		* (x2 - x1);
	glm::dvec3 v2new = v2 + massTerm * (glm::dot(v1 - v2, x1 - x2) / pow(glm::length(x1 - x2), 2))
		* (x1 - x2);

	return std::pair<glm::vec3, glm::vec3>(v1new, v2new);
}

std::pair<glm::vec3, glm::vec3> PhysicsSystem::calculateElasticCollision(const glm::dvec3& v1, const glm::dvec3& v2,
	const glm::dvec3& X1toX2, double m1, double m2)
{
	double massTerm = (2.0 * m2 / (m1 + m2));
	glm::dvec3 v1new = v1 + massTerm * (glm::dot(v2 - v1, X1toX2) / pow(glm::length(X1toX2), 2))
		* (X1toX2);
	glm::dvec3 v2new = v2 + massTerm * (glm::dot(v1 - v2, -X1toX2) / pow(glm::length(X1toX2), 2))
		* (-X1toX2);

	return std::pair<glm::vec3, glm::vec3>(v1new, v2new);
}
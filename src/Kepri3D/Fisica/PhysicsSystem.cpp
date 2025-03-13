#include "PhysicsSystem.h"

#include "Rigid.h"
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

void PhysicsSystem::update(GLuint deltaTime)
{
#ifdef __DEBUG_INFO__
	momentoTotal = { 0, 0 ,0 };
#endif
	m_deltaTime = deltaTime;
	// Comprobar que esté bien el bucle añadiendo más rigids
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
		// En el caso de los cubos, 'radio' se refiere a la longitud del lado
		float r1 = c1->radio / 2.0f;
		float r2 = c2->radio / 2.0f;
		glm::dvec3 posC1 = c1->getEntity()->getPosition();
		glm::dvec3 posC2 = c2->getEntity()->getPosition();
		
		if(posC1.x + r1 > posC2.x - r2 && posC1.x - r1 < posC2.x + r2 && // Eje X
			posC1.y + r1 > posC2.y - r2 && posC1.y - r1 < posC2.y + r2 && // Eje Y
			posC1.z + r1 > posC2.z - r2 && posC1.z - r1 < posC2.z + r2) // Eje Z
			return true;
	}
	return false;
}

void PhysicsSystem::solveCollision(Rigid* r1, Rigid* r2)
{
	// 1) Hasta que no dejen de solaparse; tirar para afuera
	unsigned int iter = 0;
	do
	{
		// TODO: dejar de detectar colisiones cuando ambos objetos ya están quietos
		r1->getEntity()->translate(-r1->m_velocity * (m_deltaTime / 1000.0));
		r2->getEntity()->translate(-r2->m_velocity * (m_deltaTime / 1000.0));
		iter++;
	} while (iter < MAX_ITER && checkCollision(r1->m_collider, r2->m_collider));

	// 2) Calcular el punto de contacto y ver qué tipo de colisión es
	glm::vec3 R1toR2 = glm::normalize(r2->getEntity()->getPosition() - r1->getEntity()->getPosition());

	// Ambos dinámicos
	if(r1->m_type == Dynamic && r2->m_type == Dynamic)
	{
		// Intercambiar velocidades y multiplicar por coef.rest.
		glm::vec3 auxVel = r1->m_velocity;
		r1->m_velocity = -R1toR2 * (float)glm::length(r2->m_velocity) * COEF_RESTITUCION;
		r2->m_velocity = R1toR2 * (float)glm::length(auxVel) * COEF_RESTITUCION;
	}
	// Uno estático; se quedan con su velocidad
	else if(r1->m_type == Static)
		r2->m_velocity = R1toR2 * (float)glm::length(r2->m_velocity) * COEF_RESTITUCION;
	else if(r2->m_type == Static)
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
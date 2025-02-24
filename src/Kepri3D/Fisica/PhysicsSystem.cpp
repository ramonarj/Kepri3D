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

void PhysicsSystem::update()
{
	// Comprobar que esté bien el bucle añadiendo más rigids
	for(int i = 0; i < m_rigids.size(); i++)
	{
		Rigid* r1 = m_rigids[i];
		for(int j = i + 1; j < m_rigids.size(); j++)
		{
			Rigid* r2 = m_rigids[j];
			// Hay colisión -> la resolvemos
			if(checkCollision(r1, r2))
			{
				//std::cout << "Colisión entre " << r1->getEntity()->getName() << " y " << r2->getEntity()->getName() << std::endl;
				solveCollision(r1, r2);
			}
		}
	}
	for(Rigid* r : m_rigids){}
	//std::cout << m_rigids.size() << std::endl;
}

bool PhysicsSystem::checkCollision(Rigid* r1, Rigid* r2)
{
	// Colisión entre 2 esferas
	//if(r1->m_collider->shape == Collider::Esfera && r2->m_collider->shape == Collider::Esfera)
	float dist = glm::length(r1->getEntity()->getPosition() - r2->getEntity()->getPosition());
	if (dist <= (r1->m_collider->radio + r2->m_collider->radio))
		return true;

	return false;
}

void PhysicsSystem::solveCollision(Rigid* r1, Rigid* r2)
{
	// Hasta que no dejen de solaparse; tirar para afuera
	do
	{
		r1->getEntity()->translate(-r1->m_velocity * 0.05);
		r2->getEntity()->translate(-r2->m_velocity * 0.05);
	} while (checkCollision(r1, r2));

	// Calcular el punto de contacto y asignar las velocidades correspondientes
	glm::vec3 R1toR2 = r2->getEntity()->getPosition() - r1->getEntity()->getPosition();
	if (r1->m_useGravity)
		r1->m_velocity = -R1toR2;
	if(r2->m_useGravity)
		r2->m_velocity = R1toR2;
	
	// TODO conservación del momento lineal
}
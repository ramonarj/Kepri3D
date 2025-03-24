#include "PhysicsSystem.h"

#include "Rigid.h"
#include "Muelle.h"
#include "Collider.h"
#include "Camera.h"
#include "Liquido.h"
#include "Articulacion.h"
#include <iostream>

PhysicsSystem* PhysicsSystem::s_instance = nullptr;

const real RAYCAST_INCR = 0.1;
vector3 PhysicsSystem::s_gravity = { 0, -9.8, 0 };

void PhysicsSystem::Clean()
{
	CleanVector(m_muelles);
	CleanVector(m_liquidos);
	CleanVector(m_articulaciones);

	delete s_instance;
	s_instance = nullptr;
}

void PhysicsSystem::addRigid(Rigid* r)
{
	Collider* col = r->getEntity()->getComponent<Collider>();
	if (col == nullptr)
		std::cout << "La entidad " << r->getEntity()->getName() << " necesita un Collider" << std::endl;
	r->setCollider(col);

	m_rigids.push_back(r);
#ifdef __DEBUG_INFO__
	if(!r->m_sleeping)
		rigidsDespiertos++;
#endif
}

void PhysicsSystem::addMuelle(Muelle* m) {
	m_muelles.push_back(m);
}

void PhysicsSystem::addLiquido(Liquido* l) {
	m_liquidos.push_back(l);
}

void PhysicsSystem::addArticulacion(Articulacion* a) {
	m_articulaciones.push_back(a);
}

void PhysicsSystem::update(float delta)
{
	m_deltaTime = delta;

	// a) Actualizar muelles
	for (Muelle* m : m_muelles)
		m->applyForce();

	// b) Actualizar líquidos
	for(Liquido* l : m_liquidos)
		l->applyBuoyancy(m_rigids);

	// c) Actualizar articulaciones
	for (Articulacion* a : m_articulaciones)
		a->applyConstraints();
#ifdef __DEBUG_INFO__
	momentoTotal = { 0, 0 ,0 };
#endif
	// d) Comprobar y resolver colisiones entre Rigids
	for(int i = 0; i < m_rigids.size(); i++)
	{
		Rigid* r1 = m_rigids[i];
#ifdef __DEBUG_INFO__
		momentoTotal += (r1->m_velocity * r1->m_mass);
#endif
		for(int j = i + 1; j < m_rigids.size(); j++)
		{
			Rigid* r2 = m_rigids[j];
			// Si ambos están dormidos, es imposible que choquen
			if (r1->m_sleeping && r2->m_sleeping) { continue; }
			// Hay colisión
			if(checkOverlap(r1->m_collider, r2->m_collider))
			{
				//std::cout << "Colisión entre " << r1->getEntity()->getName() << " y " << r2->getEntity()->getName() << std::endl;
				// Si alguno de lo 2 es trigger, no hay que resolver la colisión
				if(r1->m_collider->m_trigger || r2->m_collider->m_trigger)
				{ 
					notifyTrigger(r1->m_collider, r2->m_collider); 
				}
				else
				{
					// La resolvemos
					solveCollision(r1, r2);

					// Y notificamos a los implicados
					notifyCollision(r1->m_collider, r2->m_collider);
				}
			}
		}
	}
}

bool PhysicsSystem::checkOverlap(Collider* c1, Collider* c2)
{
	// Colisión Esfera-Esfera
	if (c1->shape == Collider::Esfera && c2->shape == Collider::Esfera)
	{
		return Collider::sphereOverlap(c1, c2);
	}
	// Colisión Cubo-Cubo (AABB)
	else if (c1->shape == Collider::Cubo && c2->shape == Collider::Cubo)
	{
		return Collider::aabbOverlap(c1, c2);
	}
	// Colisiones Esfera-Cubo
	else if (c1->shape == Collider::Esfera && c2->shape == Collider::Cubo) {
		return Collider::sphereCubeOverlap(c1, c2); 
	}
	else if (c1->shape == Collider::Cubo && c2->shape == Collider::Esfera) {
		return Collider::sphereCubeOverlap(c2, c1);
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
		r1->getEntity()->translate(-r1->m_velocity * m_deltaTime); // /2.0? para que sea mejor
		r2->getEntity()->translate(-r2->m_velocity * m_deltaTime);
		iter++;
	} while (iter < MAX_ITER && checkOverlap(r1->m_collider, r2->m_collider));

	// - - - - (2) Calcular el punto de contacto (según el tipo de los colliders) - - - - //
	vector3 R1toR2;
	// Esfera-Esfera
	if (r1->m_collider->shape == Collider::Esfera && r2->m_collider->shape == Collider::Esfera)
	{
		R1toR2 = glm::normalize(r2->getEntity()->getPosition() - r1->getEntity()->getPosition());

		// Hay que descomponer las velocidades en sus componentes normal y tangente 
		// respecto al plano de choque. La normal se transmite en la colisión, y la tangente se conserva
		vector3 newV1 = { 0, 0, 0 };
		vector3 newV2 = { 0, 0, 0 };

		// Cómo afecta el cuerpo 1 al cuerpo 2
		if (glm::length(r1->m_velocity) != 0)
		{
			real cosAlpha1 = glm::dot(glm::normalize(r1->m_velocity), R1toR2);
			vector3 velNormal = R1toR2 * glm::length(r1->m_velocity) * cosAlpha1;
			newV2 += velNormal;
			newV1 += (r1->m_velocity - velNormal); // total - normal = tangencial
		}
		// Cómo afecta el cuerpo 2 al cuerpo 1
		if (glm::length(r2->m_velocity) != 0)
		{
			real cosAlpha2 = glm::dot(glm::normalize(r2->m_velocity), -R1toR2);
			vector3 velNormal = -R1toR2 * glm::length(r2->m_velocity) * cosAlpha2;
			newV1 += velNormal;
			newV2 += (r2->m_velocity - velNormal); // total - normal = tangencial
		}

		// Si uno de ellos es estático, al otro se le devuelve la componente normal (opuesta)
		if (r1->m_type == Static) { newV2 -= newV1; }
		else if (r2->m_type == Static) { newV1 -= newV2; }

		// Asignar las nuevas velocidades
		r1->setVelocity(newV1 * COEF_RESTITUCION);
		r2->setVelocity(newV2 * COEF_RESTITUCION);
		// TODO: tener masa en cuenta
	}

	// Cubo - Cubo
	else //if (r1->m_collider->shape == Collider::Cubo && r2->m_collider->shape == Collider::Cubo)
	{
		R1toR2 = r2->getEntity()->getPosition() - r1->getEntity()->getPosition();
		vector3 absV = glm::abs(R1toR2 / r1->m_collider->halfExtents); //inspiración divina
		// No se me ocurre una forma mejor
		vector3 eje;
		if (absV.x > absV.y && absV.x > absV.z) { eje = { 1, 0, 0 }; }
		else if (absV.y > absV.x && absV.y > absV.z) { eje = { 0, 1, 0 }; }
		else { eje = { 0, 0, 1 }; }
		R1toR2 *= eje;
		R1toR2 = glm::normalize(R1toR2);

		// Ambos dinámicos
		if (r1->m_type == Dynamic && r2->m_type == Dynamic)
		{
			// prueba
			//std::pair<glm::vec3, glm::vec3> vels = calculateElasticCollision(r1->m_velocity, r2->m_velocity,
			//	R1toR2, r1->m_mass, r2->m_mass);
			//r1->m_velocity = vels.first;
			//r2->m_velocity = vels.second;

			// Intercambiar velocidades y multiplicar por coef.rest.
			vector3 auxVel = r1->m_velocity;
			r1->setVelocity(-R1toR2 * glm::length(r2->m_velocity) * COEF_RESTITUCION);
			r2->setVelocity(R1toR2 * glm::length(auxVel) * COEF_RESTITUCION);
		}
		// Uno estático; se quedan con su velocidad
		else if (r1->m_type == Static)
			r2->setVelocity(R1toR2 * glm::length(r2->m_velocity) * COEF_RESTITUCION);
		else if (r2->m_type == Static)
			r1->setVelocity(-R1toR2 * glm::length(r1->m_velocity) * COEF_RESTITUCION);
	}
}

void PhysicsSystem::notifyCollision(Collider* c1, Collider* c2)
{
	// Notificamos a las entidades involucradas (a cada componente)
	for (Component* c : c1->getEntity()->getComponents())
		c->onCollision(c2);
	for (Component* c : c2->getEntity()->getComponents())
		c->onCollision(c1);
}

void PhysicsSystem::notifyTrigger(Collider* c1, Collider* c2)
{
	// Notificamos a las entidades involucradas (a cada componente)
	for (Component* c : c1->getEntity()->getComponents())
		c->onTrigger(c2);
	for (Component* c : c2->getEntity()->getComponents())
		c->onTrigger(c1);
}

bool PhysicsSystem::raycast(const vector3& origen, const vector3& dir, real dist)
{
	real currDistance = 0;
	bool hit = false;
	// Ir avanzando poco a poco
	while(currDistance < dist && !hit)
	{
		vector3 point = origen + dir * currDistance;
		// TODO: optimizar con Octrees/alguna otra técnica
		for(Rigid* r : m_rigids)
		{
			if (r->m_collider->shape == Collider::Esfera && Collider::pointInSphere(point, r->m_collider))
				hit = true;
			else if (r->m_collider->shape == Collider::Cubo && Collider::pointInCube(point, r->m_collider))
				hit = true;
		}
		currDistance += RAYCAST_INCR;
	}
	return hit;
}

bool PhysicsSystem::raycastFromScreen(vector2 origen, real dist)
{
	// 1) Pasar de Screen Space a NDC [{800, 600} --> {-1, 1}]
	vector2 screenSize = { Game::Instance()->getCamera()->getVP()->getW(), Game::Instance()->getCamera()->getVP()->getH() };
	vector2 NDCpos = (origen / screenSize) * 2.0 - 1.0;

	// 2) Pasar de NDC a View Space
	real fov = 45.0;
	real aspectRatio = 600.0 / 800.0;
	// d = distancia focal de la cámara
	real d = 1.0 / glm::tan(fov);
	vector4 VIEWpos = { NDCpos.x / d, aspectRatio * NDCpos.y / d, -1.0, 1.0 }; //-1 por el S.coords. mano derecha
	// Método alternativo: usando la inversa de la matriz de proyección
	// VIEWpos = glm::inverse(Game::Instance()->getCamera()->getProjMat()) * NDCpos;
	
	// 3) Pasar de View Space a World Space
	vector4 WORLDpos = Game::Instance()->getCamera()->getModelMat() * VIEWpos;

	//std::cout << "{" << NDCpos.x << "," << NDCpos.y << "}" << std::endl;
	//std::cout << "{" << VIEWpos.x << "," << VIEWpos.y << "," << VIEWpos.z << "}" << std::endl;
	//std::cout << "{" << WORLDpos.x << "," << WORLDpos.y << "," << WORLDpos.z << "}" << std::endl;

	return raycast(WORLDpos, Game::Instance()->getCamera()->forward(), dist);
}

std::pair<vector3, vector3> PhysicsSystem::calculateElasticCollision(const vector3& v1, const vector3& v2,
	const vector3& x1, const vector3& x2, real m1, real m2)
{
	real massTerm = (2.0 * m2 / (m1 + m2));
	vector3 v1new = v1 + massTerm * (glm::dot(v2 - v1, x2 - x1) / pow(glm::length(x2 - x1), 2))
		* (x2 - x1);
	vector3 v2new = v2 + massTerm * (glm::dot(v1 - v2, x1 - x2) / pow(glm::length(x1 - x2), 2))
		* (x1 - x2);

	return std::pair<vector3, vector3>(v1new, v2new);
}

std::pair<vector3, vector3> PhysicsSystem::calculateElasticCollision(const vector3& v1, const vector3& v2,
	const vector3& X1toX2, real m1, real m2)
{
	real massTerm = (2.0 * m2 / (m1 + m2));
	vector3 v1new = v1 + massTerm * (glm::dot(v2 - v1, X1toX2) / pow(glm::length(X1toX2), 2))
		* (X1toX2);
	vector3 v2new = v2 + massTerm * (glm::dot(v1 - v2, -X1toX2) / pow(glm::length(X1toX2), 2))
		* (-X1toX2);

	return std::pair<vector3, vector3>(v1new, v2new);
}
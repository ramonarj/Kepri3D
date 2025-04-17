#include "PhysicsSystem.h"

#include "Rigid.h"
#include "Muelle.h"
#include "Collider.h"
#include "Camera.h"
#include "Articulacion.h"
#include "ForceGenerator.h"
#include "Scene.h"
#include <iostream>

PhysicsSystem* PhysicsSystem::s_instance = nullptr;

const real RAYCAST_INCR = 0.1;
const real MIN_STEP_TIME = 0.001; // 100 veces/segundo
vector3 PhysicsSystem::s_gravity = { 0, -9.8, 0 };

void PhysicsSystem::Clean()
{
	CleanVector(m_generators);
	CleanVector(m_muelles);
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

void PhysicsSystem::addForceGenerator(ForceGenerator* l) {
	m_generators.push_back(l);
}

void PhysicsSystem::addArticulacion(Articulacion* a) {
	m_articulaciones.push_back(a);
}

void PhysicsSystem::update(float deltaTime)
{
#ifdef FIXED_STEP_TIME
	accumTime += deltaTime;
	// Simular los pasos que haya pendientes (o ninguno, si ha sido un frame muy rápido)
	while (accumTime > m_fixedTime)
	{
		// El motor da un paso...
		simulateStep(m_fixedTime);
		// ...y los componentes suscritos a FixedUpdate dan otro
		Game::Instance()->getScene()->callFixedUpdates(m_fixedTime);
		accumTime -= m_fixedTime;
	}
#else
	// Simular exactamente 1 paso (NOTA: no se llamaría al FixedUpdate!!)
	simulateStep(deltaTime);
#endif
}

void PhysicsSystem::simulateStep(real delta)
{
	m_deltaTime = delta;
	// a) Actualizar rigids y aplicarles las fuerzas con los generadores
	for (Rigid* r : m_rigids)
	{
		r->updateStep(delta);
		// Los líquidos se incluyen aquí
		for(ForceGenerator* l : m_generators)
			l->applyForce(r, delta);
	}

	// b) Actualizar muelles
	for (Muelle* m : m_muelles)
		m->applyForce();

	// c) Actualizar articulaciones
	for (Articulacion* a : m_articulaciones)
		a->applyConstraints();
#ifdef __DEBUG_INFO__
	momentoTotal = { 0, 0 ,0 };
#endif
	// e) Detección de colisiones
	for (int i = 0; i < m_rigids.size(); i++)
	{
		Rigid* r1 = m_rigids[i];
#ifdef __DEBUG_INFO__
		momentoTotal += (r1->m_velocity * r1->m_mass);
#endif
		for (int j = i + 1; j < m_rigids.size(); j++)
		{
			Rigid* r2 = m_rigids[j];
			// Si ambos están dormidos, es imposible que choquen
			if (r1->m_sleeping && r2->m_sleeping) { continue; }
			// Si al menos uno de los 2 no tiene collider, tampoco
			if (r1->m_collider == nullptr || r2->m_collider == nullptr) { continue; }
			// Hay colisión
			if (Collider::checkOverlap(r1->m_collider, r2->m_collider))
			{
				//std::cout << "Colisión entre " << r1->getEntity()->getName() << " y " << r2->getEntity()->getName() << std::endl;
				// Si alguno de lo 2 es trigger, no hay que resolver la colisión
				if (r1->m_collider->m_trigger || r2->m_collider->m_trigger) {
					notifyTrigger(r1->m_collider, r2->m_collider);
				}
				// Crear una colisión
				else {
					createCollision(r1, r2);
				}
			}
		}
	}

	// f) Resolver todas las colisiones
	for(Colision c : m_colisiones)
	{
		// La resolvemos
		c.solveInterpenetration(m_deltaTime);
		c.solveVelocity();

		// Y notificamos a los implicados
		notifyCollision(&c);
	}
	m_colisiones.clear();
}

void PhysicsSystem::createCollision(Rigid* r1, Rigid* r2)
{
	// 1) Calcular la normal del contacto
	vector3 n;
	// Esfera-esfera
	if (r1->m_collider->shape == Collider::Esfera && r2->m_collider->shape == Collider::Esfera)
		n = glm::normalize(*r2->m_position - *r1->m_position);
	// Ortoedro-ortoedro
	else
	{
		vector3 R1toR2 = *r2->m_position - *r1->m_position;
		vector3 absV = glm::abs(R1toR2 / r1->m_collider->halfExtents); //inspiración divina
		// No se me ocurre una forma mejor
		vector3 eje;
		if (absV.x > absV.y && absV.x > absV.z) { eje = { 1, 0, 0 }; }
		else if (absV.y > absV.x && absV.y > absV.z) { eje = { 0, 1, 0 }; }
		else { eje = { 0, 0, 1 }; }
		R1toR2 *= eje;
		n = glm::normalize(R1toR2);
	}
	// TODO: profundidad del contacto
	// 2) Registrarlo
	m_colisiones.push_back(Colision(r1, r2, n));
}

real PhysicsSystem::Colision::separatingVelocity() const
{
	vector3 relativeVel = r2->m_velocity - r1->m_velocity;
	return glm::dot(relativeVel, n);
}

void PhysicsSystem::Colision::solveInterpenetration(real deltaTime)
{
	unsigned int iter = 0;
	do
	{
		// TODO: dejar de detectar colisiones cuando ambos objetos ya están quietos
		*r1->m_position -= (r1->m_velocity * deltaTime); // 2.0? para que sea mejor
		*r2->m_position -= (r2->m_velocity * deltaTime);
		iter++;
	} while (iter < MAX_ITER && Collider::checkOverlap(r1->m_collider, r2->m_collider));
}

void PhysicsSystem::Colision::solveVelocity()
{
	// 1) Calcular la nueva velocidad de separación
	real vs = separatingVelocity();

	// Contacto estacionario/separándose -> NO hace falta aplicar impulsos
	if (vs > 0) { return; }

	// Aplicar el coeficiente de restitución. TODO: arreglar choques inelásticos
	real newVs = -COEF_RESTITUCION * vs;
	real deltaVel = newVs - vs;

	// 2) Dividirla de manera proporcional a sus masas
	if (r1->m_type == Static) { 
		r2->setVelocity(r2->m_velocity + deltaVel * n);
	}
	else if (r2->m_type == Static) {
		r1->setVelocity(r1->m_velocity + deltaVel * (-n));
	}
	else 
	{
		real masaTotal = r1->m_mass + r2->m_mass;
		real prop1 = r1->m_mass / masaTotal;
		real prop2 = r2->m_mass / masaTotal;
		r1->setVelocity(r1->m_velocity + deltaVel * prop2 * (-n));
		r2->setVelocity(r2->m_velocity + deltaVel * prop1 * n);
	}
}

void PhysicsSystem::Colision::solveVelocityAntiguo()
{
	// Hay que descomponer las velocidades en sus componentes normal y tangente 
	// respecto al plano de choque. La normal se transmite en la colisión, y la tangente se conserva
	vector3 newV1 = { 0, 0, 0 };
	vector3 newV2 = { 0, 0, 0 };

	// Cómo afecta el cuerpo 1 al cuerpo 2
	if (glm::length(r1->m_velocity) != 0)
	{
		real cosAlpha1 = glm::dot(glm::normalize(r1->m_velocity), n);
		vector3 velNormal = n * glm::length(r1->m_velocity) * cosAlpha1;
		newV2 += velNormal;
		newV1 += (r1->m_velocity - velNormal); // total - normal = tangencial
	}
	// Cómo afecta el cuerpo 2 al cuerpo 1
	if (glm::length(r2->m_velocity) != 0)
	{
		real cosAlpha2 = glm::dot(glm::normalize(r2->m_velocity), -n);
		vector3 velNormal = -n * glm::length(r2->m_velocity) * cosAlpha2;
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

void PhysicsSystem::notifyCollision(Colision* col)
{
	// Notificamos a las entidades involucradas (a cada componente)
	for (Component* c : col->r1->getEntity()->getComponents())
		c->onCollision(col->r2->m_collider);
	for (Component* c : col->r2->getEntity()->getComponents())
		c->onCollision(col->r1->m_collider);
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

bool PhysicsSystem::raycast(const vector3& origen, const vector3& dir, real dist, vector3& impactPoint)
{
	real currDistance = 0;
	bool hit = false;
	Collider* collided = nullptr;
	// Ir avanzando poco a poco
	while (currDistance < dist && !hit)
	{
		impactPoint = origen + dir * currDistance;
		// TODO: optimizar con Octrees/alguna otra técnica
		for (Rigid* r : m_rigids)
		{
			if(Collider::pointInCollider(impactPoint, r->m_collider))
			{
				hit = true;
				collided = r->m_collider;
			}
		}
		currDistance += RAYCAST_INCR;
	}
	// Afinar el punto exacto de colisión (volvemos a la superficie)
	if(hit)
	{
		while (Collider::pointInCollider(impactPoint, collided))
			impactPoint -= dir * (RAYCAST_INCR / 2.0);
	}
	return hit;
}

Collider* PhysicsSystem::raycastPro(const vector3& origen, const vector3& dir, real dist)
{
	real currDistance = 0;
	bool hit = false;
	Collider* collided = nullptr;
	vector3 impactPoint;
	// Ir avanzando poco a poco
	while (currDistance < dist && !hit)
	{
		impactPoint = origen + dir * currDistance;
		// TODO: optimizar con Octrees/alguna otra técnica
		for (Rigid* r : m_rigids)
		{
			if (Collider::pointInCollider(impactPoint, r->m_collider))
			{
				hit = true;
				collided = r->m_collider;
			}
		}
		currDistance += RAYCAST_INCR;
	}
	// Afinar el punto exacto de colisión (volvemos a la superficie)
	if (hit)
	{
		while (Collider::pointInCollider(impactPoint, collided))
			impactPoint -= dir * (RAYCAST_INCR / 2.0);
		return collided;
	}
	return nullptr;
}

bool PhysicsSystem::raycastFromScreen(vector2 origen, real dist)
{
	Camera* cam = Game::Instance()->getCamera();
	// 1) Pasar de Screen Space a NDC [{800, 600} --> {-1, 1}]
	vector2 screenSize = { cam->getVP()->getW(), cam->getVP()->getH() };
	vector2 NDCpos = (origen / screenSize) * 2.0 - 1.0;

	// 2) Pasar de NDC a View Space
	vector4 VIEWpos = glm::inverse(cam->getProjMat()) * vector4(NDCpos.x, -NDCpos.y, -1.0, 1.0); //-1 por el S.coords. mano derecha

	// Método alternativo: usando los parámetros de la mat.proy. directamente
	/*
	real fov = 45.0;
	real aspectRatio = 600.0 / 800.0;
	real arInv = 1.0 / aspectRatio;
	// d = distancia focal de la cámara
	real d = 1.0 / glm::tan(fov);
	vector4 VIEWpos = { NDCpos.x / d, aspectRatio * NDCpos.y / d, -1.0, 1.0 };
	*/
	
	// 3) Pasar de View Space a World Space
	vector4 WORLDpos = cam->getModelMat() * VIEWpos;

	/*
	std::cout << "Screen Pos: " << origen.x << ", " << origen.y << std::endl;
	std::cout << "NDC Pos: {" << NDCpos.x << "," << NDCpos.y << "}" << std::endl;
	std::cout << "View Pos: {" << VIEWpos.x << "," << VIEWpos.y << "," << VIEWpos.z << "}" << std::endl;
	std::cout << "World Pos: {" << WORLDpos.x << "," << WORLDpos.y << "," << WORLDpos.z << "}" << std::endl;
	*/

	// 4) Dirección del rayo
	vector3 dir;
	if (cam->isOrto())
		dir = cam->forward();
	else
		dir = vector3(WORLDpos) - cam->getPosition();

	return raycast(WORLDpos, dir, dist);
}

Collider* PhysicsSystem::raycastFromScreenPro(vector2 origen, real dist)
{
	Camera* cam = Game::Instance()->getCamera();
	// 1) Pasar de Screen Space a NDC [{800, 600} --> {-1, 1}]
	vector2 screenSize = { cam->getVP()->getW(), cam->getVP()->getH() };
	vector2 NDCpos = (origen / screenSize) * 2.0 - 1.0;

	// 2) Pasar de NDC a View Space
	vector4 VIEWpos = glm::inverse(cam->getProjMat()) * vector4(NDCpos.x, -NDCpos.y, -1.0, 1.0); //-1 por el S.coords. mano derecha

	// 3) Pasar de View Space a World Space
	vector4 WORLDpos = cam->getModelMat() * VIEWpos;

	// 4) Dirección del rayo
	vector3 dir;
	if (cam->isOrto())
		dir = cam->forward();
	else
		dir = vector3(WORLDpos) - cam->getPosition();

	return raycastPro(WORLDpos, dir, dist);
}

void PhysicsSystem::setFixedTime(real t)
{
	m_fixedTime = glm::max(t, MIN_STEP_TIME);
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
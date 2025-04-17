#include "PhysicsMan.h"

#include "Kepri3D.h"

PhysicsMan::PhysicsMan(Rigid* r, Entity* sombra, Liquido* liquido)
{
	this->rigid = r;
	this->sombra = sombra;
	this->liquido = liquido;
}

void PhysicsMan::update(float deltaTime)
{
	// Impulsos, fuerzas y rozamiento
	controlRigid();

	// RayCast
	if (InputManager::Instance()->getKey('c'))
	{
		// Detecta el objeto colgado del muelle
		bool hit = PhysicsSystem::Instance()->raycast({ 50, -7, 15 }, { -1, 0, 0 }, 100);
		// Detecta la bola en el origen de coordenadas
		//bool hit = PhysicsSystem::Instance()->raycast({ -glm::cos(PI / 4), glm::sin(PI / 4), 5}, {0, 0, -1}, 5);
		if (hit)
			std::cout << "Chocado con algo" << std::endl;
		else
			std::cout << "Nada" << std::endl;
	}

	// RayCast from screen
	if (InputManager::Instance()->getMouseKeyDown(LEFT))
	{
		glm::ivec2 mousePos = InputManager::Instance()->getMousePos();
		// Tira un rayo desde el ratón
		bool hit = PhysicsSystem::Instance()->raycastFromScreen(mousePos, 100);

		if (hit)
			std::cout << "Chocado desde pantalla" << std::endl;
	}

	// Cambiar nivel del agua
	if (InputManager::Instance()->getKey('.')) {
		liquido->setAltura(liquido->getAltura() + deltaTime);
	}
	if (InputManager::Instance()->getKey(',')) {
		liquido->setAltura(liquido->getAltura() - deltaTime);
	}

	// Cambiar el step físico
	real fixedTime = PhysicsSystem::Instance()->getFixedTime();
	if (InputManager::Instance()->getKey('m'))
	{
		PhysicsSystem::Instance()->setFixedTime(fixedTime - deltaTime * 0.1);
	}
	if (InputManager::Instance()->getKey('n'))
	{
		PhysicsSystem::Instance()->setFixedTime(fixedTime + deltaTime * 0.1);
	}
}

void PhysicsMan::fixedUpdate(float fixedTime)
{
	if (rigid == nullptr) { return; }

	// Las fuerzas continuas (que se ejercen cada frame y no en un instante) [getKey]
	// deben aplicarse en el FixedUpdate, porque si no se acumulan de más

	// Movimiento en ejes X y Z
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_LEFT))
		rigid->addForce({ -movForce, 0, 0 });
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_RIGHT))
		rigid->addForce({ movForce, 0, 0 });
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_UP))
		rigid->addForce({ 0, 0, -movForce });
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_DOWN))
		rigid->addForce({ 0, 0, movForce });

	// Fuerzas en un punto concreto h, j
	if (InputManager::Instance()->getKey('h'))
	{
		rigid->addForce(pointForce * -rigid->getEntity()->forward(),
			rigid->getEntity()->forward() / 2.0 + rigid->getEntity()->up() / 2.0);
	}
	if (InputManager::Instance()->getKey('j'))
	{
		rigid->addForce(pointForce * -rigid->getEntity()->forward(),
			-rigid->getEntity()->forward() / 2.0 - rigid->getEntity()->up() / 2.0);
	}
}

void PhysicsMan::controlRigid()
{
	if (rigid == nullptr) { return; }

	// Los impulsos y otros eventos instantáneos (getkeyDOWN) 
	// pueden aplicarse tanto en el Update como en el FixedUpdate
	if (InputManager::Instance()->getMouseKeyDown(LEFT))
		rigid->addImpulse({ 0, jumpImpulse, 0 });

	// Torque
	if (InputManager::Instance()->getMouseKeyDown(RIGHT))
		rigid->addTorque({ 0, torque, 0 });

	// Rozamientos
	if (InputManager::Instance()->getKeyDown('t'))
		rigid->setDrag(1.5);
	if (InputManager::Instance()->getKeyDown('r'))
		rigid->setAngularDrag(0.8);

	// Poner la sombra en la entidad que tenga debajo
	glm::dvec3 origenRayo = rigid->getEntity()->getPosition() - vector3(0, rigid->getCollider()->getRadio() + 0.01, 0);
	glm::dvec3 impactPoint;
	if (PhysicsSystem::Instance()->raycast(origenRayo, { 0, -1, 0 }, 300, impactPoint))
		sombra->setPosition(impactPoint);
}
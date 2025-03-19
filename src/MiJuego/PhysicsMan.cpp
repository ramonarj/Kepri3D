#include "PhysicsMan.h"

#include "Kepri3D.h"

PhysicsMan::PhysicsMan(Rigid* r, Entity* sombra)
{
	this->rigid = r;
	this->sombra = sombra;
}

void PhysicsMan::update(GLuint deltaTime)
{
	// Movimiento en ejes X y Z
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_LEFT))
		rigid->addForce({ -movForce, 0, 0 });
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_RIGHT))
		rigid->addForce({ movForce, 0, 0 });
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_UP))
		rigid->addForce({ 0, 0, -movForce });
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_DOWN))
		rigid->addForce({ 0, 0, movForce });

	// Salto
	if (InputManager::Instance()->getMouseKeyDown(LEFT))
		rigid->addForce({ 0, jumpForce, 0 });

	// Torque
	if (InputManager::Instance()->getMouseKeyDown(RIGHT))
		rigid->addTorque({ 0, torque, 0 });

	// Rozamientos
	if (InputManager::Instance()->getKeyDown('t'))
		rigid->setDrag(2.0);

	if (InputManager::Instance()->getKeyDown('r'))
		rigid->setAngularDrag(0.8);

	// Fuerzas en un punto concreto h, j, u, n
	if (InputManager::Instance()->getKey('h'))
	{
		rigid->addForce(10.0 * -rigid->getEntity()->forward(),
			rigid->getEntity()->forward() / 2.0 + rigid->getEntity()->up() / 2.0);
	}
	if (InputManager::Instance()->getKey('j'))
	{
		rigid->addForce(10.0 * -rigid->getEntity()->forward(), 
			-rigid->getEntity()->forward() / 2.0 - rigid->getEntity()->up() / 2.0);
	}

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
		InputManager::Instance()->getMousePos();
		// Tira un rayo desde el ratón
		bool hit = PhysicsSystem::Instance()->raycastFromScreen(InputManager::Instance()->getMousePos(), 100);

		if (hit)
			std::cout << "Chocado desde pantalla" << std::endl;
	}


	// Sombra
	glm::dvec3 rigidPos = rigid->getEntity()->getPosition();
	sombra->setPosition({ rigidPos.x, 0.51, rigidPos.z });
}
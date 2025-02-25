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
		rigid->setDrag(5.0);

	if (InputManager::Instance()->getKeyDown('r'))
		rigid->setAngularDrag(0.8);

	// Sombra
	glm::dvec3 rigidPos = rigid->getEntity()->getPosition();
	sombra->setPosition({ rigidPos.x, 0.51, rigidPos.z });
}
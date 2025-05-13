#include "TPCamera.h"

#include "Kepri3D.h"

TPCamera::TPCamera(Entity* target)
{
	this->target = target;
	this->cam = cam;
}

void TPCamera::start()
{
	cam = Game::Instance()->getCamera();

	distancia = glm::length(entity->getPosition() - target->getPosition());
	cabeza = target->getChildren()[0];
}

void TPCamera::update(float deltaTime)
{
	// Control de la cámara
	//movimiento(deltaTime);
	rotaciones(deltaTime);
}

void TPCamera::fixedUpdate(float fixedTime)
{
	movimiento(fixedTime);
}

void TPCamera::movimiento(float deltaTime)
{
	//PrintVector(cabeza->forward()); std::cout << std::endl;
	glm::dvec3 fwCabeza = cabeza->forward();
	fwCabeza.y *= -1;

	entity->setPosition(target->getPosition() - distancia * fwCabeza);
}

void TPCamera::rotaciones(float deltaTime)
{
	entity->lookAt(target->getPosition());
}


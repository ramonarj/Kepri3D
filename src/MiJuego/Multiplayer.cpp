#include "Multiplayer.h"

#include "Kepri3D.h"

void Multiplayer::start()
{
	scene = Game::Instance()->getScene();
	glm::vec2 windowSize = Game::Instance()->getWindowSize();

	// Crear una nueva cámara y un nuevo viewport
	//Viewport* newVP = Game::Instance()->addViewport(windowSize.x / 2.0, windowSize.y / 2.0, 0, 0);
	//Camera* newCam = scene->addCamera(newVP);
	Camera* newCam = scene->addCamera(Game::Instance()->getViewport(0)); // usa el mismo VP existente

	// Poisicionarla
	newCam->setPosition({ 0, 20, -10 });
	newCam->lookAt({ 0, 0, 0 });

	//scene->getCamera(0)->getVP()->setPosition(400, 0);
	//scene->getCamera(0)->getVP()->setSize(400, 100);
	//scene->getCamera(1)->getVP()->setPosition(0, 0);
	//scene->getCamera(1)->getVP()->setSize(400, 300);
}

void Multiplayer::update(float deltaTime)
{
	// Cámara 1
	if(InputManager::Instance()->getKeyDown('1'))
	{
		scene->getCamera(0)->setActive(true);
		scene->getCamera(1)->setActive(false);
	}
	// Cámara 2
	else if (InputManager::Instance()->getKeyDown('2'))
	{
		scene->getCamera(0)->setActive(false);
		scene->getCamera(1)->setActive(true);
	}
}
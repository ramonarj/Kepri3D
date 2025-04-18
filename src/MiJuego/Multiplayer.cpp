#include "Multiplayer.h"

#include "Kepri3D.h"

void Multiplayer::start()
{
	scene = Game::Instance()->getScene();
	glm::vec2 windowSize = Game::Instance()->getWindowSize();

	glm::ivec2 positions[] = { {0, windowSize.y / 2.0} , {windowSize.x / 2.0, windowSize.y / 2.0},  {0, 0}, {windowSize.x / 2.0, 0} };
	for(int i = 0; i < numJugadores; i++)
	{
		// Crear una nueva cámara y un nuevo viewport
		Viewport* newVP = Game::Instance()->addViewport(windowSize.x / 2.0, windowSize.y / 2.0, positions[i].x, positions[i].y);
		Camera* newCam = scene->addCamera(newVP);

		// Poisicionarla
		if(newCam == nullptr)
		{
			std::cout << "[Multiplayer] Error: no hay soporte para mas camaras" << std::endl;
			break;
		}
		newCam->changePerspective();
		newCam->setOrtoSize(100);
		newCam->setPosition({ 0, 100, 0 });
		newCam->lookAt({ 0, 0, 0 }, { 0, 0, -1 });
	}

	scene->getCamera(0)->setActive(false);
}

void Multiplayer::update(float deltaTime)
{
	// Activar/desactivar las cámaras manualmente (teclas 0 - 5)
	for(int i = 0; i < 5; i++)
	{
		if (InputManager::Instance()->getKeyDown('0' + i))
		{
			//disableAllCameras();
			scene->getCamera(i)->setActive(!scene->getCamera(i)->isActive());
		}
	}
}

void Multiplayer::disableAllCameras()
{
	for (int i = 0; i < scene->numberOfCameras();i++)
		scene->getCamera(i)->setActive(false);
}
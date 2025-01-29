#include "GameManager.h"

#include "../Kepri3D.h"

#include "../Scene.h"
#include "CameraController.h"

#include <freeglut.h>

void GameManager::setLights(Light* dirLight, Light* circleLight, Light* spotLight)
{
	this->dirLight = dirLight;
	this->circleLight = circleLight;
	this->spotLight = spotLight;

	
	//this->dirLight->setActive(false);

	this->spotLight->setSpotlightDir({ 0, -1, 0 });
	this->spotLight->setSpotlightAngle(60.0f);
	this->spotLight->setSpotlightHardness(90);
	this->spotLight->setAttenuationFactors(0, 0.07, 0.0002);

	this->circleLight->setAttenuationFactors(1, 0, 0.05);
	this->circleLight->setActive(true);
}

void GameManager::update(GLuint deltaTime)
{
	// Salir de la aplicación / desbloquear el ratón
	if (InputManager::Instance()->getKeyDown(27)) // escape
	{
		// Desbloquear el ratón
		if (lockedMouse)
		{
			lockedMouse = false;
			// Podemos seleccionar el cursor que más nos guste!!
			InputManager::Instance()->setCursor(GLUT_CURSOR_RIGHT_ARROW);

			// Mostrar el panel de botones
			botonesMenu->setActive(true);

			// Desactivar el componente hermano que controla la cámara
			entity->getComponent<CameraController>()->setActive(false);
		}
		// Salir de la aplicación
		else
			glutLeaveMainLoop();
	}

	// Controlar las luces
	controlLuces(deltaTime);

	// Controlar la torre
	controlTorre(deltaTime);


	// Desbloquear el ratón con el clic izquierdo
	if (InputManager::Instance()->getMouseKey(LEFT))
	{
		if (!lockedMouse)
		{
			lockedMouse = true;
			InputManager::Instance()->setCursor(GLUT_CURSOR_NONE);
			InputManager::Instance()->setMousePos(cam->getVP()->getW() / 2, cam->getVP()->getH() / 2);

			// Esconder el panel de botones
			botonesMenu->setActive(false);
			// Activar el componente hermano
			entity->getComponent<CameraController>()->setActive(true);
		}	
	}


	// Modo pantalla completa (entrar / salir)
	if(InputManager::Instance()->getSpecialKeyDown(GLUT_KEY_F11))
	{
		fullscreen = !fullscreen;
		if (fullscreen)
		{
			glutFullScreen();

		}
		else
		{
			glutReshapeWindow(800, 600);
		}
	}

	// Hacer una foto
	if (InputManager::Instance()->getKeyDown('f'))
		scene->takePhoto();
}


void GameManager::controlLuces(GLuint deltaTime)
{
	// 1) Luz de foco (linterna)
	// Posición y dirección
	spotLight->setPosition(cam->getPosition());
	// Un poco feo pero funciona
	spotLight->setSpotlightDir(-cam->forward());
	// Encenderla / apagarla
	if (lockedMouse && InputManager::Instance()->getMouseKeyDown(LEFT))
	{
		spotLight->setActive(!spotLight->isActive());
	}

	// 2) Luz puntual (trayectoria circular)
	if (movingLights)
		circleLight->setPosition({ 15 * cos(totalTime / 1000.0f * 1.5), 1, 5 * sin(totalTime / 1000.0f * 1.5) });

	// 3) Luz direccional
	// Parar el movimiento de luces
	if (InputManager::Instance()->getKeyDown('t'))
	{
		movingLights = !movingLights;
	}
	// Apagarlo
	if (InputManager::Instance()->getKeyDown('l'))
	{
		dirLight->setActive(!dirLight->isActive());
	}

	// Moverlo
	if (movingLights)
	{
		dirLight->setDirection({ -cos(totalTime / 1000.0 * sunVel),
			-sin(totalTime / 1000.0 * sunVel), 0 });
		// Cambiar el color del cielo y de la luz
		float luminosidad = (dirLight->getPosition().y + 1) / 2.0f;
		Camera::setBackgroundColor(luminosidad, luminosidad, luminosidad);
		//dirLight->setDiffuse({ luminosidad, luminosidad, luminosidad, 1.0 });
	}

	totalTime += deltaTime;
}

void GameManager::controlTorre(GLuint deltaTime)
{
	glm::dvec3 movTorre = { 0,0,0 };

	// Girar a los lados
	if(InputManager::Instance()->getSpecialKey(GLUT_KEY_RIGHT))
	{
		movTorre.x += 1;
		//pSystem->rotate(3.5 * deltaTime / -1000.0, { 0,1,0 });
	}
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_LEFT))
	{
		movTorre.x -= 1;
		//pSystem->rotate(3.5 * deltaTime / 1000.0, { 0,1,0 });
	}
	// Moverse adelante / atrás
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_UP))
	{
		movTorre.z += -1;
	}
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_DOWN))
	{
		movTorre.z += 1;
	}

	// Moverse arriba / abajo
	if (InputManager::Instance()->getKey('9'))
	{
		movTorre.y += -1;
	}
	if (InputManager::Instance()->getKey('0'))
	{
		movTorre.y += 1;
	}

	// Mover la torre
	movTorre = movTorre * velTorre * (deltaTime / 1000.0);
	pSystem->translate(movTorre, GLOBAL);
}
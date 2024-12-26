#include "GameManager.h"

#include "InputManager.h"
#include "Camera.h"
#include "Scene.h"
#include "Light.h"

#include <freeglut.h>

void GameManager::update(GLuint deltaTime)
{
	// Cambiar la perspectiva
	if(InputManager::Instance()->getKeyDown('p'))
	{
		cam->changePerspective();
	}

	// Salir de la aplicación / desbloquear el ratón
	if (InputManager::Instance()->getKeyDown(27)) // escape
	{
		// Desbloquear el ratón
		if (lockedMouse)
		{
			lockedMouse = false;
			// Podemos seleccionar el cursor que más nos guste!!
			InputManager::Instance()->setCursor(GLUT_CURSOR_RIGHT_ARROW);
		}
		// Salir de la aplicación
		else
			glutLeaveMainLoop();
	}

	// Controlar las luces
	controlLuces();

	// Movimiento y rotaciones de la cámara
	if(lockedMouse)
	{
		movimientoCamara();
		rotacionesCamara();
		// Volver a dejar el ratón en el centro
		InputManager::Instance()->setMousePos(cam->getVP()->getW() / 2, cam->getVP()->getH() / 2);
	}


	// Bloquear/desbloquear el ratón
	if (InputManager::Instance()->getMouseKeyDown(LEFT))
	{
		if (!lockedMouse)
		{
			lockedMouse = true;
			InputManager::Instance()->setCursor(GLUT_CURSOR_NONE);
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
}

void GameManager::movimientoCamara()
{
	movCamara = { 0,0,0 };

	// INPUT DE TECLADO
	// Adelante / atrás
	if (InputManager::Instance()->getKeyDown('w'))
	{
		movCamara.z = 1;
	}
	if (InputManager::Instance()->getKeyDown('s'))
	{
		movCamara.z = -1;
	}
	// Izquierda / derecha
	if (InputManager::Instance()->getKeyDown('a'))
	{
		movCamara.x = -1;
	}
	if (InputManager::Instance()->getKeyDown('d'))
	{
		movCamara.x = 1;
	}
	// Arriba / abajo
	if (InputManager::Instance()->getKeyDown(32)) // barra espaciadora
	{
		movCamara.y = 1;
	}
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_SHIFT_L))
	{
		movCamara.y = -1 / 40.0;
	}

	// Input de ratón
	if (InputManager::Instance()->getMouseKeyDown(LEFT))
	{
		movCamara.z = 1 / 40.0;
	}
	else if (InputManager::Instance()->getMouseKeyDown(RIGHT))
	{
		movCamara.z = -1 / 40.0;
	}

	// Moverla en los 3 ejes
	movCamara *= velCamara;
	cam->translate({ movCamara.x, 0, movCamara.z }, LOCAL);
	cam->translate({ 0, movCamara.y, 0 }, GLOBAL);
}

void GameManager::rotacionesCamara()
{
	glm::ivec2 mousePos = InputManager::Instance()->getMousePos();
	// Incremento en la posición del ratón
	glm::dvec2 diff((double)mousePos.x - cam->getVP()->getW() / 2,
		(double)mousePos.y - cam->getVP()->getH() / 2);

	// Cámara tipo FPS; las rotaciones en Y son globales y en X son locales.
	cam->rotate(-diff.x * 0.002, { 0,1,0 }, GLOBAL);
	cam->rotate(-diff.y * 0.002, { 1,0,0 }, LOCAL); // lo mismo que hacer pitch

	// Rotar la cámara
	if (InputManager::Instance()->getMouseKeyDown(WHEEL_DOWN))
	{
		cam->rotate(0.05, { 0, 0, 1 }, LOCAL);
	}
	else if (InputManager::Instance()->getMouseKeyDown(WHEEL_UP))
	{
		cam->rotate(-0.05, { 0, 0, 1 }, LOCAL);
	}
}

void GameManager::controlLuces()
{
	glm::vec3 movLuz = { 0,0,0 };

	// Eje X
	if(InputManager::Instance()->getSpecialKey(GLUT_KEY_RIGHT))
	{
		movLuz.x = 1;
	}
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_LEFT))
	{
		movLuz.x = -1;
	}
	// Eje Z
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_UP))
	{
		movLuz.z = -1;
	}
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_DOWN))
	{
		movLuz.z = 1;
	}

	// Eje Y
	if (InputManager::Instance()->getKey('9'))
	{
		movLuz.y = -1;
	}
	if (InputManager::Instance()->getKey('0'))
	{
		movLuz.y = 1;
	}

	// Mover la luz
	movLuz *= velLuz;
	scene->getLight()->setPosition(scene->getLight()->getPosition() + movLuz);
}
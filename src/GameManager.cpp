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
	controlLuces(deltaTime);

	// Movimiento y rotaciones de la cámara
	if(lockedMouse)
	{
		movimientoCamara(deltaTime);
		rotacionesCamara();
		// Volver a dejar el ratón en el centro
		InputManager::Instance()->setMousePos(cam->getVP()->getW() / 2, cam->getVP()->getH() / 2);
	}


	// Bloquear/desbloquear el ratón
	if (InputManager::Instance()->getMouseKey(LEFT))
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

	// Hacer una foto
	if (InputManager::Instance()->getKeyDown('f'))
		scene->takePhoto();
}

void GameManager::movimientoCamara(GLuint deltaTime)
{
	movCamara = { 0,0,0 };

	// INPUT DE TECLADO
	// Adelante / atrás
	if (InputManager::Instance()->getKey('w'))
	{
		movCamara.z = 1;
	}
	if (InputManager::Instance()->getKey('s'))
	{
		movCamara.z = -1;
	}
	// Izquierda / derecha
	if (InputManager::Instance()->getKey('a'))
	{
		movCamara.x = -1;
	}
	if (InputManager::Instance()->getKey('d'))
	{
		movCamara.x = 1;
	}
	// Arriba / abajo
	if (InputManager::Instance()->getKey(32)) // barra espaciadora
	{
		movCamara.y = 1;
	}
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_SHIFT_L))
	{
		movCamara.y = -1;
	}

	// Input de ratón
	if (InputManager::Instance()->getMouseKey(LEFT))
	{
		movCamara.z = 1;
	}
	else if (InputManager::Instance()->getMouseKey(RIGHT))
	{
		movCamara.z = -1;
	}

	// Moverla en los 3 ejes
	movCamara = movCamara * (float)velCamara * (deltaTime / 1000.0f);
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
	if (InputManager::Instance()->getMouseKey(WHEEL_DOWN))
	{
		cam->rotate(0.05, { 0, 0, 1 }, LOCAL);
	}
	else if (InputManager::Instance()->getMouseKey(WHEEL_UP))
	{
		cam->rotate(-0.05, { 0, 0, 1 }, LOCAL);
	}
}

void GameManager::controlLuces(GLuint deltaTime)
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
	movLuz = movLuz * velLuz * (deltaTime / 1000.0f);
	scene->getLight()->setPosition(scene->getLight()->getPosition() + movLuz);
}
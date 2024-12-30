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
		rotacionesCamara(deltaTime);
		volumenVistaCamara(deltaTime);
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
		movCamara.z += 1;
	}
	if (InputManager::Instance()->getKey('s'))
	{
		movCamara.z += -1;
	}
	// Izquierda / derecha
	if (InputManager::Instance()->getKey('a'))
	{
		movCamara.x += -1;
	}
	if (InputManager::Instance()->getKey('d'))
	{
		movCamara.x += 1;
	}
	// Arriba / abajo
	if (InputManager::Instance()->getKey(32)) // barra espaciadora
	{
		movCamara.y += 1;
	}
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_SHIFT_L))
	{
		movCamara.y += -1;
	}

	// Input de ratón
	if (InputManager::Instance()->getMouseKey(LEFT))
	{
		movCamara.z += 1;
	}
	else if (InputManager::Instance()->getMouseKey(RIGHT))
	{
		movCamara.z += -1;
	}

	// Moverla en los 3 ejes
	movCamara = movCamara * (float)velCamara * (deltaTime / 1000.0f);
	cam->translate({ movCamara.x, 0, movCamara.z }, LOCAL);
	cam->translate({ 0, movCamara.y, 0 }, GLOBAL);
}

void GameManager::rotacionesCamara(GLuint deltaTime)
{
	glm::ivec2 mousePos = InputManager::Instance()->getMousePos();
	// Incremento en la posición del ratón
	glm::dvec2 diff((double)mousePos.x - cam->getVP()->getW() / 2,
		(double)mousePos.y - cam->getVP()->getH() / 2);

	// Cámara tipo FPS; las rotaciones en Y son globales y en X son locales.
	cam->rotate(-diff.x * 0.002, { 0,1,0 }, GLOBAL);
	cam->rotate(-diff.y * 0.002, { 1,0,0 }, LOCAL); // lo mismo que hacer pitch
}

void GameManager::volumenVistaCamara(GLuint deltaTime)
{
	if (InputManager::Instance()->getMouseKey(WHEEL_DOWN))
	{
		// Reducir el tamaño de la cámara ortográfica
		if(cam->isOrto())
			cam->setOrtoSize(cam->getOrtoSize() - (deltaTime / 1000.0f) * 100.0f);
		// Hacer zoom out (modificando el Near Plane)
		else
			cam->setNearPlane(cam->getNearPlane() + (deltaTime / 1000.0f) * 20.0f);

	}
	else if (InputManager::Instance()->getMouseKey(WHEEL_UP))
	{
		// Aumentar ""
		if (cam->isOrto())
			cam->setOrtoSize(cam->getOrtoSize() + (deltaTime / 1000.0f) * 100.0f);
		// Hacer zoom in ("")
		else
			cam->setNearPlane(cam->getNearPlane() - (deltaTime / 1000.0f) * 20.0f);
	}

	// Cambiar la distancia de renderizado (modificando el Far Plane)
	if (InputManager::Instance()->getKey('1'))
	{
		cam->setFarPlane(cam->getFarPlane() - (deltaTime / 1000.0f) * 10.0f);
	}
	else if (InputManager::Instance()->getKey('2'))
	{
		cam->setFarPlane(cam->getFarPlane() + (deltaTime / 1000.0f) * 10.0f);
	}
}

void GameManager::controlLuces(GLuint deltaTime)
{
	glm::vec3 movLuz = { 0,0,0 };

	// Eje X
	if(InputManager::Instance()->getSpecialKey(GLUT_KEY_RIGHT))
	{
		movLuz.x += 1;
	}
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_LEFT))
	{
		movLuz.x += -1;
	}
	// Eje Z
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_UP))
	{
		movLuz.z += -1;
	}
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_DOWN))
	{
		movLuz.z += 1;
	}

	// Eje Y
	if (InputManager::Instance()->getKey('9'))
	{
		movLuz.y += -1;
	}
	if (InputManager::Instance()->getKey('0'))
	{
		movLuz.y += 1;
	}

	// Mover la luz
	movLuz = movLuz * velLuz * (deltaTime / 1000.0f);
	scene->getLight()->setPosition(scene->getLight()->getPosition() + movLuz);
}
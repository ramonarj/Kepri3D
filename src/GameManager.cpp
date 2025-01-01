#include "GameManager.h"

#include "InputManager.h"
#include "Camera.h"
#include "Scene.h"
#include "Light.h"

#include <freeglut.h>

void GameManager::setLights(Light* dirLight, Light* circleLight, Light* spotLight)
{
	this->dirLight = dirLight;
	this->circleLight = circleLight;
	this->spotLight = spotLight;

	
	this->dirLight->setActive(false);

	this->spotLight->setSpotlightDir({ 0, -1, 0 });
	this->spotLight->setSpotlightAngle(60.0f);
	this->spotLight->setSpotlightHardness(90);
	this->spotLight->setAttenuationFactors(0, 0.07, 0.0002);

	this->circleLight->setAttenuationFactors(1, 0, 0.05);
}

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
	// 1) Luz de foco (linterna)
	//glm::vec3 movLuz = { 0,0,0 };

	// Eje X
	//if(InputManager::Instance()->getSpecialKey(GLUT_KEY_RIGHT))
	//{
	//	movLuz.x += 1;
	//}
	//if (InputManager::Instance()->getSpecialKey(GLUT_KEY_LEFT))
	//{
	//	movLuz.x += -1;
	//}
	//// Eje Z
	//if (InputManager::Instance()->getSpecialKey(GLUT_KEY_UP))
	//{
	//	movLuz.z += -1;
	//}
	//if (InputManager::Instance()->getSpecialKey(GLUT_KEY_DOWN))
	//{
	//	movLuz.z += 1;
	//}

	//// Eje Y
	//if (InputManager::Instance()->getKey('9'))
	//{
	//	movLuz.y += -1;
	//}
	//if (InputManager::Instance()->getKey('0'))
	//{
	//	movLuz.y += 1;
	//}

	// Mover la luz
	//movLuz = movLuz * velLuz * (deltaTime / 1000.0f);
	//pointLight->setPosition(pointLight->getPosition() + movLuz);

	// 1) Luz de foco (linterna)
	spotLight->setPosition(cam->getPosition());
	// Un poco feo pero funciona
	spotLight->setSpotlightDir(glm::fvec3{ -cam->getModelMat()[2][0],
		-cam->getModelMat()[2][1], -cam->getModelMat()[2][2] });
	// Encenderla / apagarla
	if (lockedMouse && InputManager::Instance()->getMouseKeyDown(LEFT))
	{
		spotLight->setActive(!spotLight->isActive());
	}

	// 2) Luz puntual (trayectoria circular)
	circleLight->setPosition({ 15 * cos(totalTime * 0.002), 1, 5 * sin(totalTime * 0.002) });

	// 3) Luz direccional
	if (InputManager::Instance()->getKeyDown('l'))
		dirLight->setActive(!dirLight->isActive());
	dirLight->setDirection({ -cos(totalTime * 0.0001),-sin(totalTime * 0.0001), 0 });
	totalTime += deltaTime;
}
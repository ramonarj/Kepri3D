#include "CameraController.h"

#include "Kepri3D.h"
#include "Scenes/PruebaFisicas.h"
#include "Scenes/PruebaScene.h"

CameraController::CameraController(Camera* cam)
{
	this->cam = cam;
	cam->setFarPlane(1000.0);

	// Hacer que el cursor sea invisible y moverlo al centro de la ventana
	InputManager::Instance()->setCursor(GLUT_CURSOR_NONE);
	InputManager::Instance()->setMousePos(cam->getVP()->getW() / 2, cam->getVP()->getH() / 2);
}

void CameraController::update(GLuint deltaTime)
{
	// Cambiar/recargar la escena
	if (InputManager::Instance()->getSpecialKeyDown(GLUT_KEY_F1))
		Game::Instance()->loadScene(new PruebaScene());
	else if (InputManager::Instance()->getSpecialKeyDown(GLUT_KEY_F2))
		Game::Instance()->loadScene(new PruebaFisicas());

	// Cambiar la perspectiva
	if (InputManager::Instance()->getKeyDown('p'))
	{
		cam->changePerspective();
	}

	// Control de la cámara
	movimientoCamara(deltaTime);
	rotacionesCamara(deltaTime);
	volumenVistaCamara(deltaTime);
	puntosFijosCamara();

	// Devolver el ratón al medio
	InputManager::Instance()->setMousePos(cam->getVP()->getW() / 2, cam->getVP()->getH() / 2);
}

void CameraController::movimientoCamara(GLuint deltaTime)
{
	glm::vec3 movCamara = { 0,0,0 };

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
	movCamara = movCamara * (float)velocidad * (deltaTime / 1000.0f);
	cam->translate({ movCamara.x, 0, movCamara.z }, LOCAL);
	cam->translate({ 0, movCamara.y, 0 }, GLOBAL);
}

void CameraController::rotacionesCamara(GLuint deltaTime)
{
	glm::vec2 rotCamara = { 0,0 };

	// Incremento en la posición del ratón
	glm::ivec2 mousePos = InputManager::Instance()->getMousePos();
	glm::dvec2 diff((double)mousePos.x - cam->getVP()->getW() / 2,
		(double)mousePos.y - cam->getVP()->getH() / 2);

	// Aplicarle la sensibilidad
	rotCamara -= diff;
	rotCamara *= (5 / 1000.0 * sensibilidad);

	// Cámara tipo FPS; las rotaciones en Y son globales y en X son locales.
	cam->rotate(rotCamara.x, { 0,1,0 }, GLOBAL);
	cam->rotate(rotCamara.y, { 1,0,0 }, LOCAL); // lo mismo que hacer pitch
}

void CameraController::volumenVistaCamara(GLuint deltaTime)
{
	if (InputManager::Instance()->getMouseKey(WHEEL_DOWN))
	{
		// Reducir el tamaño de la cámara ortográfica
		if (cam->isOrto())
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
		cam->setFarPlane(cam->getFarPlane() - (deltaTime / 1000.0f) * 20.0f);
	}
	else if (InputManager::Instance()->getKey('2'))
	{
		cam->setFarPlane(cam->getFarPlane() + (deltaTime / 1000.0f) * 20.0f);
	}
}

void CameraController::puntosFijosCamara()
{
	float dist = 25;
	// Izquierda/derecha
	if (InputManager::Instance()->getKeyDown('4'))
	{
		cam->setPosition({ -dist, 0, 0 });
		cam->lookAt(cam->getPosition() + glm::dvec3(1, 0, 0));
	}
	if (InputManager::Instance()->getKeyDown('6'))
	{
		cam->setPosition({ dist, 0, 0 });
		cam->lookAt(cam->getPosition() + glm::dvec3(-1, 0, 0));
	}
	// Arriba/abajo
	if (InputManager::Instance()->getKeyDown('8'))
	{
		cam->setPosition({ 0, dist, 0 });
		cam->lookAt(cam->getPosition() + glm::dvec3(0, -1, 0), { 0, 0, -1 });
	}
	if (InputManager::Instance()->getKeyDown('2'))
	{
		cam->setPosition({ 0, -10, 0 });
		cam->lookAt(cam->getPosition() + glm::dvec3(0, 1, 0), { 0, 0, 1 });
	}
	// Adelante/atrás
	if (InputManager::Instance()->getKeyDown('5'))
	{
		cam->setPosition({ 0, 0, dist });
		cam->lookAt(cam->getPosition() + glm::dvec3(0, 0, -1));
	}
	if (InputManager::Instance()->getKeyDown('1'))
	{
		cam->setPosition({ 0, 0, -dist });
		cam->lookAt(cam->getPosition() + glm::dvec3(0, 0, 1));
	}
}
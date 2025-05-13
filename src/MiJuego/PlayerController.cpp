#include "PlayerController.h"

#include "Kepri3D.h"

PlayerController::PlayerController()
{
}

void PlayerController::start()
{
	rigid = entity->getComponent<Rigid>();
	if (rigid == nullptr)
		std::cout << "El jugador necesita un Rigid" << std::endl;
	cabeza = entity->getChildren()[0];
}

void PlayerController::update(float deltaTime)
{
	movimiento(deltaTime);
	rotaciones(deltaTime);

	// Devolver el ratón al medio
	Viewport* vp = Game::Instance()->getCamera()->getVP();
	InputManager::Instance()->setMousePos(vp->getW() / 2, vp->getH() / 2);
}

void PlayerController::movimiento(float deltaTime)
{
	glm::vec3 mov = { 0,0,0 };

	// Adelante / atrás										
	if (InputManager::Instance()->getKey('w')) {
		mov.z += 1;
	}
	if (InputManager::Instance()->getKey('s')) {
		mov.z -= 1;
	}

	// Izquierda / derecha
	if (InputManager::Instance()->getKey('a')) {
		mov.x += 1;
	}
	if (InputManager::Instance()->getKey('d')) {
		mov.x -= 1;
	}

	// Saltar (barra espaciadora)
	if (InputManager::Instance()->getKeyDown(32)) {
		if(!jumping) {
			rigid->addImpulse({ 0, 1000, 0 });
			jumping = true;
		}
	}

	// Moverla en los 3 ejes
	mov = mov * (float)velocidad * deltaTime;
	entity->translate(mov, LOCAL);
}

void PlayerController::rotaciones(float deltaTime)
{
	glm::vec2 rotacion = { 0,0 };

	Viewport* vp = Game::Instance()->getCamera()->getVP();

	// Incremento en la posición del ratón
	glm::ivec2 mousePos = InputManager::Instance()->getMousePos();
	glm::dvec2 diff((double)mousePos.x - vp->getW() / 2,
		(double)mousePos.y - vp->getH() / 2);

	// Aplicarle la sensibilidad
	rotacion -= diff;
	rotacion *= (5 / 1000.0 * sensibilidad);

	// Rotar el cuerpo
	// Cámara tipo FPS; las rotaciones en Y son globales y en X son locales.
	entity->rotate(rotacion.x, { 0,1,0 }, GLOBAL);

	// Rotar la cabeza
	cabeza->rotate(rotacion.y, { 1,0,0 }, LOCAL);
}

void PlayerController::onCollision(Collider* other)
{
	//std::cout << "Suelo" << std::endl;
	jumping = false;
}
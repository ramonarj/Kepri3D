#ifndef __MULTIPLAYER__
#define __MULTIPLAYER__

#include "Component.h"
class Scene;

/* Gestiona las cámaras y puertos de vista de un multijugador local a pantalla partida */
class Multiplayer : public Component
{
public:
	Multiplayer(int numJugadores) : numJugadores(numJugadores), scene(nullptr) {}
	~Multiplayer(){}

	void start() override;
	void update(float deltaTime) override;
private:
	int numJugadores;
	Scene* scene;

	//
	void disableAllCameras();
};

#endif
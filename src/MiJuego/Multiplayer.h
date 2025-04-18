#ifndef __MULTIPLAYER__
#define __MULTIPLAYER__

#include "Component.h"
class Scene;

class Multiplayer : public Component
{
public:
	Multiplayer(int numJugadores) : numJugadores(numJugadores){}
	~Multiplayer(){}

	void start() override;
	void update(float deltaTime) override;
private:
	int numJugadores;
	Scene* scene;
};

#endif
#ifndef __PLAYER_CONTROLLER__
#define __PLAYER_CONTROLLER__

#include "Component.h"

class Rigid;

class PlayerController : public Component
{
public:
	PlayerController();
	~PlayerController(){}

	void start() override;
	void update(float deltaTime) override;
	void onCollision(Collider* other) override;
private:
	double velocidad = 20.0f;
	float sensibilidad = 0.25;
	Rigid* rigid;
	bool jumping = false;
	Entity* cabeza;

	// Métodos
	void movimiento(float deltaTime);
	void rotaciones(float deltaTime);
};
#endif

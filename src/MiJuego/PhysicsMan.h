#ifndef __PHYSICS_MAN__
#define __PHYSICS_MAN__

#include "Component.h"

class Liquido;
class Rigid;

class PhysicsMan : public Component
{
public:
	PhysicsMan(Rigid* r, Entity* sombra, Liquido* liquido);
	~PhysicsMan() {}

	void update(float deltaTime) override;
private:
	Rigid* rigid;
	Entity* sombra;
	Liquido* liquido;

	float movForce = 15.0f * 20;// *2000;
	float jumpForce = 6000.0f * 20;// *2000.0f; // impulso
	float torque = 300.0f; //impulso
};

#endif

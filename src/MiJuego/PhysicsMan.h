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

	float movForce = 100.0f;// *2000;
	float jumpImpulse = 10.0f * 300000; // impulso
	float torque = 300.0f; //impulso
};

#endif

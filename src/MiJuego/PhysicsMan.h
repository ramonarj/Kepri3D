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
	void fixedUpdate(float fixedTime) override;
private:
	Rigid* rigid;
	Entity* sombra;
	Liquido* liquido;
	
	double pointForce = 10.0;// *300000;
	float movForce = 80.0f * 1000;// *300000;
	float jumpImpulse = 10.0f * 1000; // 300000; // impulso
	float torque = 300.0f; //impulso

	// Metodos
	void controlRigid();
};

#endif

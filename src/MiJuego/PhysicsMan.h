#ifndef __PHYSICS_MAN__
#define __PHYSICS_MAN__

#include "Component.h"

class Rigid;

class PhysicsMan : public Component
{
public:
	PhysicsMan(Rigid* r, Entity* sombra);
	~PhysicsMan() {}

	void update(GLuint deltaTime) override;
private:
	Rigid* rigid;
	Entity* sombra;

	float movForce = 15.0f;
	float jumpForce = 6000.0f;
	float torque = 1.0f;
};

#endif

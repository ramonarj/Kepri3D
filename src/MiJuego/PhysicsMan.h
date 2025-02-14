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

	float movForce = 10.0f;
	float jumpForce = 7000.0f;
};

#endif

#ifndef __VIENTO__
#define __VIENTO__

#include "ForceGenerator.h"

class Viento : public ForceGenerator
{
public:
	Viento(glm::vec3 vel = { 1, 0, 0 }) : vel(vel) {}
	~Viento(){}

	void applyForce(Particle* p, real deltaTime) override;
	void applyForce(Rigid* p, real deltaTime) override;
private:
	glm::vec3 vel;
};

#endif
#include "Viento.h"

#include "Kepri3D.h"

void Viento::applyForce(Particle* p, real deltaTime)
{
	// TODO: caparlo a un volumen concreto
	if(p->position->y > -10) // que no afecte si la partícula está bajo el agua
		p->velocity += (vector3)vel * deltaTime;
}

void Viento::applyForce(Rigid* r, real deltaTime)
{
	r->addForce((vector3)vel * deltaTime);
}
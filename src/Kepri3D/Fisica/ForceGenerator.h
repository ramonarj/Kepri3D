#ifndef __FORCE_GENERATOR__
#define __FORCE_GENERATOR__

// Basado en la estructura del libro "Game Physics Engine Development" de Ian Millington
#include "CorePhysics.h"
struct Particle;
class Rigid;

/* Clase abstracta (en esencia). Aplica una fuerza a implementar durante cada frame a una partícula/sólido rígido */
class ForceGenerator
{
public:
	// Versión para partículas
	virtual void applyForce(Particle* p, real deltaTime) {}
	// Versión para sólidos rígidos
	virtual void applyForce(Rigid* p, real deltaTime) {}
private:
};

#endif

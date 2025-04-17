#ifndef __FORCE_GENERATOR__
#define __FORCE_GENERATOR__

// Basado en la estructura del libro "Game Physics Engine Development" de Ian Millington
#include "CorePhysics.h"
struct Particle;
class Rigid;

/* Clase abstracta (en esencia). Aplica una fuerza a implementar durante cada frame a una part�cula/s�lido r�gido */
class ForceGenerator
{
public:
	// Versi�n para part�culas
	virtual void applyForce(Particle* p, real deltaTime) {}
	// Versi�n para s�lidos r�gidos
	virtual void applyForce(Rigid* p, real deltaTime) {}
private:
};

#endif

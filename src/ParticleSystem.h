#ifndef __PARTICLE_SYSTEM__
#define __PARTICLE_SYSTEM__

#include "Entity.h"
#include <string>

class ParticleSystem : public Entity
{
public:
	ParticleSystem(const std::string& particleTextureId, GLuint numParticles); 
	~ParticleSystem();

	void render(const glm::dmat4& viewMat) override;

private:
	/* N�mero de part�culas */
	GLuint numParticles;

	/* Posici�n de cada part�cula */
	glm::dvec3* m_positions;
};

// - - - - - - - - - - 


#endif
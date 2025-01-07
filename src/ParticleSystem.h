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
	/* Número de partículas */
	GLuint numParticles;

	/* Posición de cada partícula */
	glm::dvec3* m_positions;
};

// - - - - - - - - - - 


#endif
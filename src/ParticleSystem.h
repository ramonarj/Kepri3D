#ifndef __PARTICLE_SYSTEM__
#define __PARTICLE_SYSTEM__

#include "Entity.h"
#include <string>

class Camera;

enum PARTICLE_TYPE
{
	PARTICLE_2D = 0,
	PARTICLE_3D = 1
};


class ParticleSystem : public Entity
{
public:
	ParticleSystem(GLdouble size, GLuint maxParticles, PARTICLE_TYPE partType = PARTICLE_2D); 
	~ParticleSystem();

	void render(const glm::dmat4& viewMat) override;

	void update(GLuint deltaTime) override;

	// Setters
	/* Establece la rapidez de las partículas */
	inline void setParticleSpeed(float speed) { this->m_particleSpeed = speed; }
	/* Establece el tiempo de vida (en segundos) de las partículas */
	void setLifetime(double time);

private:
	/* Número actual de partículas activas */
	//GLuint numParticles;

	/* Número máximo de partículas */
	GLuint maxParticles;

	/* Rapidez de las partículas */
	float m_particleSpeed;

	/* Tamaño de las partículas */


	/* Tiempo de vida de las partículas */
	GLuint m_maxLifetime;

	/* Dirección de la emisión */
	glm::dvec3 emissionDir;

	// ----- Específico de cada partícula ----- //
	/* Posición de cada partícula */
	glm::dvec3* m_positions;

	/* Velocidad (normalizada) de cada partícula */
	glm::vec3* m_velocities;

	/* Tiempo que lleva viva cada partícula */
	GLuint* m_life;
};

// - - - - - - - - - - 


#endif
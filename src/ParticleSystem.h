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
	/* Establece la rapidez de las part�culas */
	inline void setParticleSpeed(float speed) { this->m_particleSpeed = speed; }
	/* Establece el tiempo de vida (en segundos) de las part�culas */
	void setLifetime(double time);

private:
	/* N�mero actual de part�culas activas */
	//GLuint numParticles;

	/* N�mero m�ximo de part�culas */
	GLuint maxParticles;

	/* Rapidez de las part�culas */
	float m_particleSpeed;

	/* Tama�o de las part�culas */


	/* Tiempo de vida de las part�culas */
	GLuint m_maxLifetime;

	/* Direcci�n de la emisi�n */
	glm::dvec3 emissionDir;

	// ----- Espec�fico de cada part�cula ----- //
	/* Posici�n de cada part�cula */
	glm::dvec3* m_positions;

	/* Velocidad (normalizada) de cada part�cula */
	glm::vec3* m_velocities;

	/* Tiempo que lleva viva cada part�cula */
	GLuint* m_life;
};

// - - - - - - - - - - 


#endif
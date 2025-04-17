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

enum EMISSION_TYPE { SPHERE = 0, CIRCLE = 1, CONE = 2, VOLUMETRIC = 3 };


class ParticleSystem : public Entity
{
public:
	ParticleSystem(GLdouble size, GLuint maxParticles, EMISSION_TYPE = SPHERE, PARTICLE_TYPE partType = PARTICLE_2D); 
	~ParticleSystem();

	void render() override;

	void update(float deltaTime) override;

	// Setters
	void setParticleSpeed(float speed);
	void setLifetime(double time);
	inline void useGravity(bool use) { m_useGravity = use; }
	void setBurst(bool burst);
	inline void setLoop(bool loop) { m_loop = loop; }

private:
	/* Forma de la emisión */
	EMISSION_TYPE m_emissionType;

	/* Número máximo de partículas */
	GLuint maxParticles;

	/* Rapidez de las partículas */
	float m_particleSpeed;

	// ¿Usan gravedad?
	bool m_useGravity = false;

	// ¿Ráfagas o flujo emisión contante?
	bool m_burst = false;
	bool m_loop = true;

	/* Tiempo de vida de las partículas, en segundos */
	float m_maxLifetime;

	/* Dirección de la emisión */
	glm::dvec3 emissionDir;

	// ----- Específico de cada partícula ----- //
	/* Posición de cada partícula */
	glm::dvec3* m_positions;

	/* Velocidad (normalizada) de cada partícula */
	glm::vec3* m_velocities;

	/* Tiempo que lleva viva cada partícula */
	float* m_life;

	// Metodos
	void assignStartingPosition(int i);
	void assignStartingVelocity(int i);
};

// - - - - - - - - - - 


#endif
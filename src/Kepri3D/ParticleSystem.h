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
	/* Forma de la emisi�n */
	EMISSION_TYPE m_emissionType;

	/* N�mero m�ximo de part�culas */
	GLuint maxParticles;

	/* Rapidez de las part�culas */
	float m_particleSpeed;

	// �Usan gravedad?
	bool m_useGravity = false;

	// �R�fagas o flujo emisi�n contante?
	bool m_burst = false;
	bool m_loop = true;

	/* Tiempo de vida de las part�culas, en segundos */
	float m_maxLifetime;

	/* Direcci�n de la emisi�n */
	glm::dvec3 emissionDir;

	// ----- Espec�fico de cada part�cula ----- //
	/* Posici�n de cada part�cula */
	glm::dvec3* m_positions;

	/* Velocidad (normalizada) de cada part�cula */
	glm::vec3* m_velocities;

	/* Tiempo que lleva viva cada part�cula */
	float* m_life;

	// Metodos
	void assignStartingPosition(int i);
	void assignStartingVelocity(int i);
};

// - - - - - - - - - - 


#endif
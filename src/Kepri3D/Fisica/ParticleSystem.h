#ifndef __PARTICLE_SYSTEM__
#define __PARTICLE_SYSTEM__

#include "Entity.h"
#include "CorePhysics.h"

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
	inline void setGravity(real gravity) { m_gravity = gravity; m_useGravity = true; }
	void setVolume(const vector3& vol);

private:
	struct Particle
	{
		vector3* position;
		vector3 velocity;
		real life;
	};

	/* Lista de part�culas */
	Particle* m_particles;

	/* N�mero m�ximo de part�culas */
	GLuint maxParticles;

	// - - - - EMISI�N - - - - //
	/* Forma de la emisi�n */
	EMISSION_TYPE m_emissionType;

	// �R�fagas o emisi�n contante?
	bool m_burst = false;
	bool m_loop = true;

	/* Direcci�n/volumen de la emisi�n */
	union
	{
		vector3 emissionDir;
		vector3 m_volume = { 2, 8, 2 };
	};

	/* Tiempo de vida de las part�culas, en segundos */
	real m_maxLifetime;

	// - - - - FISICAS - - - - //
	/* Rapidez de las part�culas */
	real m_particleSpeed;

	// �Usan gravedad?
	bool m_useGravity = false;
	real m_gravity = -1.0;

	// - - - - RENDERIZADO - - - - //
	// Necesitamos esto para dibujar instanciado
	vector3* m_positions;


	// ++++ Metodos ++++ //
	void assignStartingPosition(int i);
	void assignStartingVelocity(int i);
	void killParticle(int i);
};

// - - - - - - - - - - 

#endif
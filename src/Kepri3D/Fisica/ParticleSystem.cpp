#include "ParticleSystem.h"

#include "Mesh.h"
#include "Renderer.h"
#include "PhysicsSystem.h"

ParticleSystem::ParticleSystem(GLdouble size, GLuint maxParticles, EMISSION_TYPE emisssionType, PARTICLE_TYPE partType)
{
	m_name = "ParticleSystem";

	// Generar la malla y renderer
	if (partType == PARTICLE_2D) // 'círculo'
		m_renderer = new Renderer(Mesh::generateRectangle(size, size));
	else // esfera
		m_renderer = new Renderer(IndexMesh::generateSphere(size, 5, 12));
	addComponent(m_renderer);

	// Asignar el shader específico
	setShader("particle");

	this->m_emissionType = emisssionType;
	this->maxParticles = maxParticles;

	// Valores por defecto
	if (m_emissionType == VOLUMETRIC)
		this->m_volume = { 10, 10, 10 };
	else
		this->emissionDir = { 0, -1, 0 };
	this->m_particleSpeed = 1.5f;
	this->m_maxLifetime = 3.0f;

	// Lista de partículas
	m_particles = new Particle[maxParticles];
	// Las posiciones se crean contiguas en otra parte de la memoria, para poder mandarlas juntas al shader
	m_positions = new vector3[maxParticles];
	
	// Valores iniciales
	for (int i = 0; i < maxParticles; i++)
	{
		m_particles[i].life = (m_maxLifetime / (real)maxParticles) * i;
		assignStartingPosition(i);
		assignStartingVelocity(i);
	}
}

void ParticleSystem::setBurst(bool burst)
{
	m_burst = burst;
	if(burst)
	{
		for (int i = 0; i < maxParticles; i++)
			m_particles[i].life = 0;
	}
}

void ParticleSystem::setVolume(const vector3& vol)
{
	m_volume = vol;

	for (int i = 0; i < maxParticles; i++)
		assignStartingPosition(i);
}

void ParticleSystem::setParticleSpeed(float speed) 
{
	this->m_particleSpeed = speed;
	for (int i = 0; i < maxParticles; i++)
	{
		m_particles[i].velocity *= speed;
	}
}

void ParticleSystem::setLifetime(double time)
{
	this->m_maxLifetime = time;
	for (int i = 0; i < maxParticles; i++)
	{
		m_particles[i].life = (m_maxLifetime / (real)maxParticles) * i;
	}
}


ParticleSystem::~ParticleSystem()
{
	delete[] m_particles;
	delete[] m_positions;
	// ?
}

void ParticleSystem::render()
{
	// Definir la forma de pintar la malla
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);

	// Cargar el material al shader
	getMaterial()->loadToShader(getMaterial()->getShader());

	// 2) Dibujar las mallas de todas las partículas a la vez
	// TODO: dejar dde pintar las partículas muertas
	if (m_renderer != nullptr)
	{
		// Para el vertex shader; posición de cada partícula
		glVertexAttribPointer(10, 3, GL_DOUBLE, GL_FALSE, 0, m_positions);
		glEnableVertexAttribArray(10);
		// Para decirle al vertex shader que las posiciones (location = 10 en este caso) 
		// corresponden a 1 por instancia, y no por vértice
		// (0 = cada vértice, 1 = cada instancia, 2 = cada 2 instancias, etc.)
		glVertexAttribDivisor(10, 1); 

		m_renderer->getMesh()->drawInstanced(maxParticles);

		glDisableVertexAttribArray(10);
	}
}


void ParticleSystem::update(float deltaTime)
{
	// TODO: usar una 'Thread pool'
	// Actualizar la posición y el tiempo de vida de cada partícula
	for (int i = 0; i < maxParticles; i++)
	{
		if (m_particles[i].life > m_maxLifetime) { continue; } // partícula muerta

		// Aplicar gravedad
		if (m_useGravity && m_particles[i].velocity.y > -150.0)
			m_particles[i].velocity.y += m_gravity * deltaTime;

		// Actualizar posición y vida
		*m_particles[i].position += (m_particles[i].velocity * (real)deltaTime);
		m_particles[i].life += deltaTime;

		// Partícula se muere de vieja
		if(m_particles[i].life > m_maxLifetime)
		{
			killParticle(i);
		}
	}
}

void ParticleSystem::assignStartingPosition(int i)
{
	m_particles[i].position = &m_positions[i]; // trucaso para no repetir datos

	if(m_emissionType == VOLUMETRIC)
	{
		m_positions[i].x = ((rand() % 1000) / 500.0 - 1) * m_volume.x;
		m_positions[i].y = ((rand() % 1000) / 500.0 - 1) * m_volume.y;
		m_positions[i].z = ((rand() % 1000) / 500.0 - 1) * m_volume.z;

		m_positions[i] += getPosition();
		return;
	}
	m_positions[i] = getPosition();
}

void ParticleSystem::assignStartingVelocity(int i)
{
	real dirX, dirY, dirZ;
	switch (m_emissionType)
	{
	case SPHERE:
	{
		// Velocidad aleatoria, la cual conserva durante toda la simulación
		dirX = rand() % 1000 / 500.0 - 1;
		dirY = rand() % 1000 / 500.0 - 1;
		dirZ = rand() % 1000 / 500.0 - 1;
		break;
	}
	case CIRCLE:
	{
		dirX = rand() % 1000 / 500.0 - 1;
		dirY = 0;
		dirZ = rand() % 1000 / 500.0 - 1;
		break;
	}
	case CONE:
	{
		real ang = (rand() % 360) * PI / 180.0;
		dirX = cos(ang);
		dirY = -1;
		dirZ = sin(ang);
		break;
	}
	case VOLUMETRIC:
	{
		dirX = rand() % 1000 / 500.0 - 1;
		dirY = -1;
		dirZ = rand() % 1000 / 500.0 - 1;
		break;
	}
	default:
		break;
	}
	m_particles[i].velocity = glm::normalize(vector3({ dirX, dirY, dirZ }));
	m_particles[i].velocity *= m_particleSpeed;
}

void ParticleSystem::killParticle(int i)
{
	// La revivimos (devolverla al estado inicial
	if (m_loop)
	{
		m_particles[i].life = 0;
		assignStartingPosition(i);
		//assignStartingVelocity(i); // depende de para qué
	}
	else
	{
		assignStartingVelocity(i);
	}
}
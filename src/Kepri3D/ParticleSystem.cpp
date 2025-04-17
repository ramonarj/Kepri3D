#include "ParticleSystem.h"

#include "Mesh.h"
#include "Texture.h"
#include "Camera.h"
#include "Shader.h"
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
	this->emissionDir = { 0, -1, 0 };
	this->m_particleSpeed = 1.5f;
	this->m_maxLifetime = 3.0f;

	// Específico de cada partícula
	// Tiempo de vida (segundos)
	m_life = new float[maxParticles];
	// Posición
	m_positions = new glm::dvec3[maxParticles];
	// Velocidad normalizada
	m_velocities = new glm::vec3[maxParticles];
	
	// Valores iniciales
	for (int i = 0; i < maxParticles; i++)
	{
		m_life[i] = ((double)m_maxLifetime / maxParticles) * i;
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
			m_life[i] = 0;
	}
}

void ParticleSystem::setParticleSpeed(float speed) 
{
	this->m_particleSpeed = speed;
	for (int i = 0; i < maxParticles; i++)
	{
		m_velocities[i] *= speed;
	}
}

void ParticleSystem::setLifetime(double time)
{
	this->m_maxLifetime = time;
	for (int i = 0; i < maxParticles; i++)
	{
		m_life[i] = (m_maxLifetime / (float)maxParticles) * i;
	}
}


ParticleSystem::~ParticleSystem()
{
	if (m_positions != nullptr)
		delete[] m_positions;
	if (m_velocities != nullptr)
		delete[] m_velocities;
	if (m_life != nullptr)
		delete[] m_life;
}

void ParticleSystem::render()
{
	// Definir la forma de pintar la malla
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);

	// Activar la textura si la tiene
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
		if (m_life[i] > m_maxLifetime) { continue; } // partícula muerta

		//Gravedad
		if (m_useGravity && m_velocities[i].y > -150.0)
			m_velocities[i] += vector3(0, -9.8, 0) * (real)deltaTime;

		// Posición y vida
		m_positions[i] += (m_velocities[i] * deltaTime );
		m_life[i] += deltaTime;

		// Partícula se muere
		if(m_life[i] > m_maxLifetime)
		{
			// La revivimos (devolverla al estado inicial
			if(m_loop)
			{
				m_life[i] = 0;
				assignStartingPosition(i);
				//assignStartingVelocity(i); // depende de para qué
			}
			else
			{
				assignStartingVelocity(i);
			}
		}
	}
}

void ParticleSystem::assignStartingPosition(int i)
{
	if(m_emissionType == VOLUMETRIC)
	{
		float volumeSize = 50.0;
		m_positions[i].x = ((rand() % 1000) / 500.0 - 1) * volumeSize;
		m_positions[i].y = ((rand() % 1000) / 500.0 - 1) * volumeSize;
		m_positions[i].z = ((rand() % 1000) / 500.0 - 1) * volumeSize;

		m_positions[i] += getPosition();
		return;
	}
	m_positions[i] = getPosition();
}

void ParticleSystem::assignStartingVelocity(int i)
{
	float dirX, dirY, dirZ;
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
		float ang = (rand() % 360) * PI / 180;
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
	m_velocities[i] = glm::normalize(glm::vec3({ dirX, dirY, dirZ }));
	m_velocities[i] *= m_particleSpeed;
}
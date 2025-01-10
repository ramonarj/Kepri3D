#include "ParticleSystem.h"

#include "Mesh.h"
#include "Texture.h"
#include "Camera.h"
#include "Shader.h"
#include <gtc/type_ptr.hpp>

ParticleSystem::ParticleSystem(const std::string& particleTextureId, GLdouble size, GLuint maxParticles, PARTICLE_TYPE partType)
{
	//m_mesh = IndexMesh::generateRectangle(1, 1);
	// Generar la malla
	if (partType == PARTICLE_2D) // 'círculo'
		m_mesh = Mesh::generateFilledPolygon(10, size);
	else // esfera
		m_mesh = IndexMesh::generateSphere(size, 12, true);

	// Asignar la textura y el shader específico
	setTexture(particleTextureId);
	setShader("particle");

	// Valores por defecto
	this->maxParticles = maxParticles;
	this->emissionDir = { 0, -1, 0 };
	this->m_particleSpeed = 1.5;
	this->m_maxLifetime = 3000.0f;

	// Específico de cada partícula
	// Tiempo de vida
	m_life = new GLuint[maxParticles];
	// Posición
	m_positions = new glm::dvec3[maxParticles];
	// Velocidad normalizada
	m_velocities = new glm::vec3[maxParticles];
	
	// Valores iniciales
	for (int i = 0; i < maxParticles; i++)
	{
		m_life[i] = ((double)m_maxLifetime / maxParticles) * i;
		m_positions[i] = getPosition();

		// Velocidad aleatoria, la cual conserva durante toda la simulación
		float dirX = rand() % 1000 / 500.0 - 1;
		float dirY = rand() % 1000 / 500.0 - 1;
		float dirZ = rand() % 1000 / 500.0 - 1;
		m_velocities[i] = glm::normalize(glm::dvec3({ dirX, dirY, dirZ }));
	}
}

void ParticleSystem::setLifetime(double time)
{
	this->m_maxLifetime = time * 1000;
	for (int i = 0; i < maxParticles; i++)
	{
		m_life[i] = ((double)m_maxLifetime / maxParticles) * i;
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

void ParticleSystem::render(const glm::dmat4& viewMat)
{
	// Definir la forma de pintar la malla
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);

	// Activar la textura si la tiene
	if (m_texture != nullptr)
		m_texture->bind();

	m_material.load();
	// 2) Dibujar las mallas de todas las partículas a la vez
	if (m_mesh != nullptr)
	{
		// Uniform (P * V)
		int viewProjLoc = glGetUniformLocation(m_shader->getId(), "viewProjMat");
		glUniformMatrix4dv(viewProjLoc, 1, GL_FALSE, glm::value_ptr(m_cam->getProjMat() * viewMat));

		// Para el vertex shader; posición de cada partícula
		glVertexAttribPointer(10, 3, GL_DOUBLE, GL_FALSE, 0, m_positions);
		glEnableVertexAttribArray(10);
		// Para decirle al vertex shader que las posiciones (location = 10 en este caso) 
		// corresponden a 1 por instancia, y no por vértice
		// (0 = cada vértice, 1 = cada instancia, 2 = cada 2 instancias, etc.)
		glVertexAttribDivisor(10, 1); 

		m_mesh->drawInstanced(maxParticles);

		glDisableVertexAttribArray(10);
	}
		

	// Desactivar la textura si la tiene
	if (m_texture != nullptr)
		m_texture->unbind();


	// Dejarlo como estaba
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


void ParticleSystem::update(GLuint deltaTime)
{
	// Actualizar la posición y el tiempo de vida de cada partícula
	for (int i = 0; i < maxParticles; i++)
	{
		m_positions[i] += (m_velocities[i] * m_particleSpeed * (deltaTime / 1000.0f));
		m_life[i] += deltaTime;

		// Partícula se muere -> devolverla al estado inicial
		if(m_life[i] > m_maxLifetime)
		{
			m_life[i] = 0;
			m_positions[i] = getPosition();
		}
	}
}
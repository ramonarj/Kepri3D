#include "ParticleSystem.h"

#include "Mesh.h"
#include "Texture.h"
#include <gtc/type_ptr.hpp>

ParticleSystem::ParticleSystem(const std::string& particleTextureId, GLuint numParticles)
{
	// Generar la malla y asignar la textura
	//m_mesh = IndexMesh::generateRectangle(0.5, 0.5);
	m_mesh = IndexMesh::generateSphere(0.25, 20, false);
	//setTexture(particleTextureId);
	setShader("particle");
	this->numParticles = numParticles;

	m_positions = new glm::dvec3[numParticles];
	// Todas las partículas formando un gran cubo
	int count = 0;
	int cubeRoot = std::pow(numParticles, 1.0 / 3);
	for(int i = 0; i < cubeRoot; i++)
	{
		for (int j = 0; j < cubeRoot; j++)
		{
			for (int k = 0; k < cubeRoot; k++)
			{
				m_positions[count] = { i , j , k };
				count++;
			};
		};
	};
	std::cout << count << " partículas en escena" << std::endl;
}


ParticleSystem::~ParticleSystem()
{
	if (m_positions != nullptr)
		delete[] m_positions;
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
		// Para el vertex shader; posición de cada partícula
		glVertexAttribPointer(10, 3, GL_DOUBLE, GL_FALSE, 0, m_positions);
		glEnableVertexAttribArray(10);
		// Para decirle al vertex shader que las posiciones (location = 10 en este caso) 
		// corresponden a 1 por instancia, y no por vértice
		// (0 = cada vértice, 1 = cada instancia, 2 = cada 2 instancias, etc.)
		glVertexAttribDivisor(10, 1); 

		m_mesh->drawInstanced(numParticles);

		glDisableVertexAttribArray(10);
	}
		

	// Desactivar la textura si la tiene
	if (m_texture != nullptr)
		m_texture->unbind();


	// Dejarlo como estaba
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

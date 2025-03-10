#include "Renderer.h"

#include "Mesh.h"

Renderer::Renderer(Mesh* mesh)
{
	m_mesh = mesh;

	// Valores por defecto
	m_polyModeFront = GL_FILL;
	m_polyModeBack = GL_FILL;

	m_receiveShadows = true;
	m_castShadows = true;
}

Renderer::~Renderer()
{
	delete m_mesh;
}

void Renderer::draw()
{
	if (!m_active) { return; }

	glPolygonMode(GL_FRONT, m_polyModeFront);
	glPolygonMode(GL_BACK, m_polyModeBack);

	m_mesh->draw();
}

void Renderer::setPolygonMode(GLenum front, GLenum back)
{
	m_polyModeFront = front;
	m_polyModeBack = back;
}
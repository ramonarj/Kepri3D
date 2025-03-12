#include "Renderer.h"

#include "Mesh.h"
#include "Shader.h"
#include "ResourceManager.h"

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

void Renderer::drawFixed()
{
	if (!m_active) { return; }

	glPolygonMode(GL_FRONT, m_polyModeFront);
	glPolygonMode(GL_BACK, m_polyModeBack);

	m_material.load();

	m_mesh->draw();
}

void Renderer::draw(Shader* sh)
{
	if (!m_active) { return; }

	glPolygonMode(GL_FRONT, m_polyModeFront);
	glPolygonMode(GL_BACK, m_polyModeBack);

	// Cargar material
	m_material.loadToShader(sh);

	// Sombras
	sh->setInt("receive_shadows", m_receiveShadows);

	m_mesh->draw();
}

void Renderer::setPolygonMode(GLenum front, GLenum back)
{
	m_polyModeFront = front;
	m_polyModeBack = back;
}

void Renderer::setMaterial(const std::string& materialID)
{
	m_material = ResourceManager::Instance()->getMaterial(materialID);
}
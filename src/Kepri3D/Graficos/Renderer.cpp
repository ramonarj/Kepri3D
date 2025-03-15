#include "Renderer.h"

#include "Mesh.h"
#include "Shader.h"
#include "ResourceManager.h"

bool Renderer::visibleBounds = true;

Renderer::Renderer(Mesh* mesh)
{
	m_mesh = mesh;

	// Bounding Volume
	m_mesh->calculateVolume();
	glm::dvec3 size = m_mesh->getVolume();
	m_volumeMesh = IndexMesh::generateCube(1);
	m_volumeMesh->scale(size);
	// TODO: optimizar y no usar normales ni uvs en el volume mesh

	// Valores por defecto
	m_polyModeFront = GL_FILL;
	m_polyModeBack = GL_FILL;

	m_receiveShadows = true;
	m_castShadows = true;
}

Renderer::~Renderer()
{
	delete m_mesh;
	if (m_volumeMesh != nullptr)
		delete m_volumeMesh;
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

	// Debug volume
	if (visibleBounds) { drawVolume(sh); }
}

void Renderer::drawVolume(Shader* sh)
{
	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_LINE);
	m_volumeMesh->draw();
}

void Renderer::setPolygonMode(GLenum front, GLenum back)
{
	m_polyModeFront = front;
	m_polyModeBack = back;
}

void Renderer::setMaterial(const std::string& materialID)
{
	m_material = ResourceManager::Instance()->getMaterial(materialID);
	// Calcular las tangentes si es necesario
	if(m_material.needsTangents())
		static_cast<IndexMesh*>(m_mesh)->setTangents();
}
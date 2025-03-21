#include "Renderer.h"

#include "Mesh.h"
#include "Shader.h"
#include "ResourceManager.h"

bool Renderer::visibleBounds = true;

Renderer::Renderer(Mesh* mesh) : m_materials(nullptr)
{
	m_mesh = mesh;

	// Bounding Volume
	m_mesh->calculateVolume();
	glm::dvec3 size = m_mesh->getVolume();
	m_volumeMesh = IndexMesh::generateCube(1);
	m_volumeMesh->scale(size);
	dynamic_cast<IndexMesh*>(m_volumeMesh)->calculateSubmeshes();
	// TODO: optimizar y no usar normales ni uvs en el volume mesh

	// Submeshes y materiales
	IndexMesh* m = dynamic_cast<IndexMesh*>(mesh);
	if (m != nullptr)
	{
		m->calculateSubmeshes();
		m_numSubmallas = m->numSubmallas();
		m_materials = new Material[m_numSubmallas];
	}
	else
	{
		m_numSubmallas = 1;
		m_materials = new Material[1];
	}

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
	if (m_materials != nullptr)
		delete[] m_materials;
}

void Renderer::drawFixed()
{
	if (!m_active) { return; }

	glPolygonMode(GL_FRONT, m_polyModeFront);
	glPolygonMode(GL_BACK, m_polyModeBack);

	m_materials[0].load();

	m_mesh->draw();
}

void Renderer::draw(Shader* sh)
{
	if (!m_active) { return; }

	glPolygonMode(GL_FRONT, m_polyModeFront);
	glPolygonMode(GL_BACK, m_polyModeBack);

	// Sombras
	sh->setInt("receive_shadows", m_receiveShadows);

	// Mallas sin indexar
	IndexMesh* indMesh = dynamic_cast<IndexMesh*>(m_mesh); // temporal
	if(indMesh == nullptr || m_numSubmallas == 1)
	{
		m_materials[0].loadToShader(sh);
		m_mesh->draw();
	}

	else
	{
		// Dibujar las submallas
		for (int i = 0; i < m_numSubmallas; i++)
		{
			// Cargar material en cuestión
			m_materials[i].loadToShader(sh);

			// Dibujar submalla
			indMesh->drawSubmesh(i);
		}
	}

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
	setMaterial(0, materialID);
}

void Renderer::setMaterial(int i, const std::string& materialID)
{
	// TODO controlar errores
	m_materials[i] = ResourceManager::Instance()->getMaterial(materialID);

	// Calcular las tangentes si es necesario
	if (m_materials[0].needsTangents())
		static_cast<IndexMesh*>(m_mesh)->setTangents();
}
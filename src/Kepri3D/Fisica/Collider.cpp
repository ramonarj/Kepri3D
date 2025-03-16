#include "Collider.h" 

#include "Mesh.h"
#include "Renderer.h"

Collider::Collider(float radio)
{
	this->shape = Esfera;
	this->radio = radio;

	// renderer
	m_renderer = new Renderer(IndexMesh::IndexMesh::generateSphere(radio, 10, 15));

	// wireframe
	m_renderer->setPolygonMode(GL_LINE, GL_LINE);
	m_visible = false;
	m_trigger = false;
}	

Collider::Collider(const glm::dvec3& size)
{
	this->shape = Cubo;
	this->halfExtents = size / 2.0;

	// renderer
	IndexMesh* m = IndexMesh::generateCube(1.0);
	m->scale(size);
	m_renderer = new Renderer(m);

	// wireframe
	m_renderer->setPolygonMode(GL_LINE, GL_LINE);
	m_visible = false;
	m_trigger = false;
}

Collider::~Collider()
{
	delete m_renderer;
}

void Collider::render(Shader* sh)
{
	if (m_visible) { m_renderer->draw(sh); }
}
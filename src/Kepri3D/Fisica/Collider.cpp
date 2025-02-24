#include "Collider.h" 

#include "Mesh.h"
#include "Renderer.h"

Collider::Collider(Shape forma, float radio) 
{
	this->shape = forma;
	this->radio = radio;

	if (forma == Esfera)
		m_renderer = new Renderer(IndexMesh::IndexMesh::generateSphere(radio, 10, 15));
	else if (forma == Cubo)
		m_renderer = new Renderer(IndexMesh::generateCube(radio));

	// wireframe
	m_renderer->setPolygonMode(GL_LINE, GL_LINE);
	m_visible = false;
}	

Collider::~Collider()
{
	delete m_renderer;
}

void Collider::render()
{
	if (m_visible) { m_renderer->draw(); }
}
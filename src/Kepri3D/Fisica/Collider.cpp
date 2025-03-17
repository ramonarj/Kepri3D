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

// - - - - - - - - - - - - - - - - - - - 

bool Collider::sphereOverlap(const Collider* c1, const Collider* c2)
{
	// Distancia entre ambos centros
	float dist = glm::length(c1->getEntity()->getPosition() - c2->getEntity()->getPosition());
	// Hay overlap
	if (dist < (c1->radio + c2->radio))
		return true;

	return false;
}

bool Collider::aabbOverlap(const Collider* c1, const Collider* c2)
{
	glm::dvec3 halfExt1 = c1->halfExtents;
	glm::dvec3 halfExt2 = c2->halfExtents;
	glm::dvec3 posC1 = c1->getEntity()->getPosition();
	glm::dvec3 posC2 = c2->getEntity()->getPosition();

	// Hay overlap
	if (posC1.x + halfExt1.x > posC2.x - halfExt2.x && posC1.x - halfExt1.x < posC2.x + halfExt2.x && // Eje X
		posC1.y + halfExt1.y > posC2.y - halfExt2.y && posC1.y - halfExt1.y < posC2.y + halfExt2.y && // Eje Y
		posC1.z + halfExt1.z > posC2.z - halfExt2.z && posC1.z - halfExt1.z < posC2.z + halfExt2.z) // Eje Z
		return true;

	return false;
}

bool Collider::sphereCubeOverlap(const Collider* sphere, const Collider* cube)
{
	double dist_squared = sphere->radio * sphere->radio;
	glm::dvec3 posEsf = sphere->getEntity()->getPosition();
	// C1 Y C2 son 2 esquinas opuestas del cubo
	glm::dvec3 C1 = cube->getEntity()->getPosition() - cube->halfExtents;
	glm::dvec3 C2 = cube->getEntity()->getPosition() + cube->halfExtents;

	// Eje X
	if (posEsf.x < C1.x)
		dist_squared -= pow(posEsf.x - C1.x, 2);
	else if (posEsf.x > C2.x)
		dist_squared -= pow(posEsf.x - C2.x, 2);
	// Eje Y
	if (posEsf.y < C1.y)
		dist_squared -= pow(posEsf.y - C1.y, 2);
	else if (posEsf.y > C2.y)
		dist_squared -= pow(posEsf.y - C2.y, 2);
	// Eje Z
	if (posEsf.z < C1.z)
		dist_squared -= pow(posEsf.z - C1.z, 2);
	else if (posEsf.z > C2.z)
		dist_squared -= pow(posEsf.z - C2.z, 2);

	return dist_squared > 0;
}

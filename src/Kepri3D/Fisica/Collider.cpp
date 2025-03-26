#include "Collider.h" 

#include "Mesh.h"
#include "Renderer.h"

Collider::Collider(real radio)
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

Collider::Collider(const vector3& size)
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


bool Collider::checkOverlap(const Collider* c1, const Collider* c2)
{
	// Colisión Esfera-Esfera
	if (c1->shape == Collider::Esfera && c2->shape == Collider::Esfera)
		return Collider::sphereOverlap(c1, c2);

	// Colisión Cubo-Cubo (AABB)
	else if (c1->shape == Collider::Cubo && c2->shape == Collider::Cubo)
		return Collider::aabbOverlap(c1, c2);

	// Colisiones Esfera-Cubo
	else if (c1->shape == Collider::Esfera && c2->shape == Collider::Cubo) {
		return Collider::sphereCubeOverlap(c1, c2);
	}
	else if (c1->shape == Collider::Cubo && c2->shape == Collider::Esfera) {
		return Collider::sphereCubeOverlap(c2, c1);
	}
	// TODO: colliders con otras formas ("custom")

	return false;
}

bool Collider::sphereOverlap(const Collider* c1, const Collider* c2)
{
	// Distancia entre ambos centros
	real dist = glm::length(c1->getEntity()->getPosition() - c2->getEntity()->getPosition());
	// Hay overlap
	if (dist < (c1->radio + c2->radio))
		return true;

	return false;
}

bool Collider::aabbOverlap(const Collider* c1, const Collider* c2)
{
	vector3 halfExt1 = c1->halfExtents;
	vector3 halfExt2 = c2->halfExtents;
	vector3 posC1 = c1->getEntity()->getPosition();
	vector3 posC2 = c2->getEntity()->getPosition();

	// Hay overlap
	if (posC1.x + halfExt1.x > posC2.x - halfExt2.x && posC1.x - halfExt1.x < posC2.x + halfExt2.x && // Eje X
		posC1.y + halfExt1.y > posC2.y - halfExt2.y && posC1.y - halfExt1.y < posC2.y + halfExt2.y && // Eje Y
		posC1.z + halfExt1.z > posC2.z - halfExt2.z && posC1.z - halfExt1.z < posC2.z + halfExt2.z) // Eje Z
		return true;

	return false;
}

bool Collider::sphereCubeOverlap(const Collider* sphere, const Collider* cube)
{
	real dist_squared = sphere->radio * sphere->radio;
	vector3 posEsf = sphere->getEntity()->getPosition();
	// C1 Y C2 son 2 esquinas opuestas del cubo
	vector3 C1 = cube->getEntity()->getPosition() - cube->halfExtents;
	vector3 C2 = cube->getEntity()->getPosition() + cube->halfExtents;

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

bool Collider::pointInCollider(const vector3& point, const Collider* col)
{
	if (col->shape == Collider::Esfera)
		return pointInSphere(point, col);
	else if(col->shape == Collider::Cubo)
		return pointInCube(point, col);
}

bool Collider::pointInCube(const vector3& point, const Collider* cube)
{
	vector3 halfExt = cube->halfExtents;
	vector3 cubePos = cube->getEntity()->getPosition();

	// Hay overlap
	if (cubePos.x + halfExt.x >= point.x && cubePos.x - halfExt.x <= point.x && // Eje X
		cubePos.y + halfExt.y >= point.y && cubePos.y - halfExt.y <= point.y && // Eje Y
		cubePos.z + halfExt.z >= point.z && cubePos.z - halfExt.z <= point.z) // Eje Z
		return true;

	return false;
}

bool Collider::pointInSphere(const vector3& point, const Collider* sphere)
{
	real dist = glm::length(sphere->getEntity()->getPosition() - point);

	return (dist <= sphere->radio);
}

// - - - - - - - - - - - - - - - 

real Collider::volume() const
{
	real volumen = -1;
	//  - - Ortoedros - - // -> V = lado * lado * lado
	if (shape == Cubo)
	{
		//vector3 fullExtents = halfExtents * 2.0;
		//volumen = fullExtents.x * fullExtents.y * fullExtents.z;
		volumen = 8.0 * (halfExtents.x * halfExtents.y * halfExtents.z); // más rápido
	}
	// - - Esferas - - // -> V = 4 / 3 * PI * r3
	else
	{
		volumen = (4.0 / 3.0) * PI * glm::pow(radio, 3);
	}
	return volumen;
}
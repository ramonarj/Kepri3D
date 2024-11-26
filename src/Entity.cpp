#include "Entity.h"
#include <glut.h>

using namespace glm;

Entity::Entity()
{
}

void Entity::render()
{
	if (m_mesh != nullptr)
		m_mesh->draw();
}

// - - - - - - - - - - - - - - - - - 

EjesRGB::EjesRGB(GLdouble l)
{
	m_mesh = Mesh::generateAxesRGB(l);
}

// - - - - - - - - - - - - - - - - - 

Poligono::Poligono(GLint sides, bool relleno)
{
	if(relleno)
		m_mesh = Mesh::generateFilledPolygon(sides);
	else
		m_mesh = Mesh::generatePolygon(sides);
}

// - - - - - - - - - - - - - - - - - 
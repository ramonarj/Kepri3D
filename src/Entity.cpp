#include "Entity.h"
#include <glut.h>
#include <iostream>

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

void EjesRGB::render()
{
	// También podría hacerse con esto
	/*int windowW = glutGet(GLUT_WINDOW_WIDTH);
	int windowH = glutGet(GLUT_WINDOW_HEIGHT);*/

	// Obtener información del puerto de vista
	GLint m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport); // x, y, w, h

	// Cambiarlo a nuestro antojo y pintar la entidad
	glViewport(0, m_viewport[3] / 2, m_viewport[2] / 2, m_viewport[3] / 2);
	Entity::render();

	// Volver a dejarlo como estaba
	glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
}

// - - - - - - - - - - - - - - - - - 

Poligono::Poligono(GLint sides, bool relleno)
{
	if(relleno)
		m_mesh = Mesh::generateFilledPolygon(sides);
	else
		m_mesh = Mesh::generatePolygon(sides);
}

void Poligono::render()
{
	GLint m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport); 

	glViewport(m_viewport[2] / 2, m_viewport[3] / 2, m_viewport[2] / 2, m_viewport[3] / 2);
	//glPolygonMode(GL_FRONT, GL_LINE); // -> si quisiéramos que los triángulos no se rellenen, solo las líneas

	Entity::render();

	glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // el predeterminado
}

// - - - - - - - - - - - - - - - - - 
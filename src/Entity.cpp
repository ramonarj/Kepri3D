#include "Entity.h"
#include <glut.h>

#include <gtc/type_ptr.hpp>

//#define GLM_ENABLE_EXPERIMENTAL
//#include <gtx/string_cast.hpp>
//#include <gtc/matrix_access.hpp>

#include <iostream>

using namespace glm;


void Entity::render(glm::dmat4 const& viewMat)
{
	// 1) Cargar la matriz M*V
	// Decirle a OpenGL que la siguiente matriz que cargaremos es de modelado/vista (no de proyección)
	glMatrixMode(GL_MODELVIEW);
	// Calcular la matriz MV (modelado*vista)
	glm::dmat4 modelViewMat = viewMat * modelMat;
	// Cargarla 
	glLoadMatrixd(value_ptr(modelViewMat));
	//glLoadMatrixd((GLdouble*)&modelViewMat); //equivalente a hacer esto


	// 2) Dibujar la/s malla/s
	if (m_mesh != nullptr)
		m_mesh->draw();
}

void Entity::update(GLuint timeElapsed)
{
	// *) Prueba de transformaciones de la matriz de modelado
	// Traslaciones
	//modelMat = glm::translate(modelMat, dvec3(timeElapsed / 10000.0, 0.0, 0.0));

	// Rotaciones
	//modelMat = glm::rotate(modelMat, timeElapsed / 1000.0, glm::dvec3(1, 1, 1));

	// Escalado
	//modelMat = glm::scale(modelMat, glm::dvec3(0.99, 0.99, 0.99));

	//PrintMatrix(&modelMat);
}

void Entity::setPosition(glm::dvec3 pos)
{
	modelMat = glm::translate(modelMat, pos);
}

void Entity::rotate(GLdouble alpha, glm::dvec3 axis)
{
	modelMat = glm::rotate(modelMat, alpha, axis);
}


// - - - - - - - - - - - - - - - - - 

EjesRGB::EjesRGB(GLdouble l)
{
	m_mesh = Mesh::generateAxesRGB(l);
}

// - - - - - - - - - - - - - - - - - 

Poligono::Poligono(GLint sides, GLdouble size, bool relleno, std::string textureName)
{
	if(relleno)
	{
		// Cargar la textura
		if (textureName != "" && !m_texture.load(textureName))
			std::cout << "CARGA DE TEXTURA INCORRECTA\n";

		m_mesh = Mesh::generateFilledPolygon(sides, size);
	}

	else
		m_mesh = Mesh::generatePolygon(sides, size);
}

void Poligono::render(glm::dmat4 const& viewMat)
{
	glCullFace(GL_FRONT);
	m_texture.bind();

	glPolygonMode(GL_FRONT, GL_FILL); // -> si quisiéramos que los triángulos no se rellenen, solo las líneas

	Entity::render(viewMat);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // el predeterminado

	m_texture.unbind();
}

// - - - - - - - - - - - - - - - - - 

Cubo::Cubo(GLdouble size)
{
	m_mesh = Mesh::generateCube(size);
}

void Cubo::render(glm::dmat4 const& viewMat)
{
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glPolygonMode(GL_BACK, GL_FILL);
	Entity::render(viewMat);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // el predeterminado
}

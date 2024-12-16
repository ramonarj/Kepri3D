#include "Entity.h"
#include <glut.h>

#include <gtc/type_ptr.hpp>

//#define GLM_ENABLE_EXPERIMENTAL
//#include <gtx/string_cast.hpp>
//#include <gtc/matrix_access.hpp>

#include <iostream>
#include "Utils.h"

using namespace glm;


void Entity::render(glm::dmat4 const& viewMat)
{
	// 1) Cargar la matriz V*M
	// Decirle a OpenGL que la siguiente matriz que cargaremos es de modelado/vista (no de proyección)
	glMatrixMode(GL_MODELVIEW);
	// Calcular la matriz VM (vista * modelado)
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
	// Cambiamos la última columna, que contiene la posición
	modelMat[3][0] = pos.x;
	modelMat[3][1] = pos.y;
	modelMat[3][2] = pos.z;
	
	//PrintMatrix(&modelMat);
}

void Entity::translate(glm::dvec3 transVector, ReferenceSystem refSys)
{
	dmat4 transMat = glm::translate({ 1.0 }, transVector);

	// Traslación local: postmultiplicamos la matriz de traslación
	if(refSys == LOCAL)
	{
		modelMat = modelMat * transMat;
	}
	// Traslación global: premultiplicamos " "
	else if(refSys == GLOBAL)
	{
		modelMat = transMat * modelMat;
	}
}

void Entity::rotate(GLdouble alpha, glm::dvec3 axis, ReferenceSystem refSys)
{
	// Rotación local: postmultiplicamos la matriz de rotación
	if(refSys == LOCAL)
	{
		// por ahora todas las rotaciones son locales
		modelMat = glm::rotate(modelMat, alpha, axis);
	}
	// Rotación global: premultiplicamos la matriz de rotación
	else if (refSys == GLOBAL)
	{
		// Guardarnos la posición previa
		dvec3 pos = { modelMat[3][0], modelMat[3][1], modelMat[3][2] };

		// Rotar respecto al eje global
		dmat4 rotMatrix = glm::rotate(dmat4(1.0), alpha, axis);
		modelMat = rotMatrix * modelMat;

		// Devolver la entidad a su sitio
		setPosition(pos);
	}
	//PrintMatrix(&modelMat);
}

void Entity::scale(glm::dvec3 scale)
{
	modelMat = glm::scale(modelMat, scale);
}


// - - - - - - - - - - - - - - - - - 

EjesRGB::EjesRGB(GLdouble l)
{
	m_mesh = Mesh::generateAxesRGB(l);
}

// - - - - - - - - - - - - - - - - - 

Poligono::Poligono(GLint sides, GLdouble size, bool relleno)
{
	if(relleno)
		m_mesh = Mesh::generateFilledPolygon(sides, size);
	else
		m_mesh = Mesh::generatePolygon(sides, size);
}

void Poligono::render(glm::dmat4 const& viewMat)
{

	m_texture.bind();
	glPolygonMode(GL_FRONT, GL_FILL); // -> si quisiéramos que los triángulos no se rellenen, solo las líneas

	Entity::render(viewMat);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // el predeterminado
	m_texture.unbind();
}

// - - - - - - - - - - - - - - - - - 

Cubo::Cubo(GLdouble size, bool textured, bool equalFaces)
{
	m_mesh = IndexMesh::generateCube(size, textured, equalFaces);
}

void Cubo::render(glm::dmat4 const& viewMat)
{
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);
	m_texture.bind();

	Entity::render(viewMat);

	m_texture.unbind();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // el predeterminado
}

void Cubo::update(GLuint deltaTime)
{
	rotate(deltaTime * 0.001, { 0,1,0 }, LOCAL);
}


// - - - - - - - - - - - - - - - - - 

Esfera::Esfera(GLdouble size, GLuint subdivisions, bool textured)
{
	m_mesh = IndexMesh::generateSphere(size, subdivisions, textured);
}

void Esfera::render(glm::dmat4 const& viewMat)
{
	glDisable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);
	m_texture.bind();
	Entity::render(viewMat);
	m_texture.unbind();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // el predeterminado

	//glEnable(GL_CULL_FACE);
}

void Esfera::update(GLuint timeElapsed)
{
	//rotate(timeElapsed * 0.001, { 0,1,0 }, LOCAL);
}

// - - - - - - - - - - - - - - - - - 

Grid::Grid(GLuint filas, GLuint columnas, GLdouble tamFila, GLdouble tamColumna)
{
	m_mesh = IndexMesh::generateGrid(filas, columnas, tamFila, tamColumna);
}

void Grid::render(glm::dmat4 const& viewMat)
{
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);
	Entity::render(viewMat);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // el predeterminado
}

// - - - - - - - - - - - - - - - - - 

Terrain::Terrain(std::string filename, GLdouble scale)
{
	m_mesh = IndexMesh::generateTerrain(filename, scale);
}

void Terrain::render(glm::dmat4 const& viewMat)
{
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);
	m_texture.bind();
	Entity::render(viewMat);
	m_texture.unbind();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // el predeterminado
}


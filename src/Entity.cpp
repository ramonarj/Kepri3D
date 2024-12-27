#include "Entity.h"

#include <iostream>

#include <gtc/type_ptr.hpp>

#include "Utils.h"
#include "ResourceManager.h"
#include "Mesh.h"
#include "Texture.h"
#include "MeshLoader.h"

using namespace glm;


void Entity::render(glm::dmat4 const& viewMat)
{
	// Para materiales translúcidos
	if (m_material.isTranslucid())
	{
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glDepthMask(GL_FALSE);
	}

	// Definir la forma de pintar la malla
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);

	// Activar la textura si la tiene
	if (m_texture != nullptr)
		m_texture->bind();

	// 1) Cargar la matriz V*M
	// Decirle a OpenGL que la siguiente matriz que cargaremos es de modelado/vista (no de proyección)
	glMatrixMode(GL_MODELVIEW);
	// Calcular la matriz VM (vista * modelado)
	glm::dmat4 modelViewMat = viewMat * modelMat;
	// Cargarla 
	glLoadMatrixd(value_ptr(modelViewMat));
	//glLoadMatrixd((GLdouble*)&modelViewMat); //equivalente a hacer esto


	// 2) Dibujar la/s malla/s
	m_material.load();
	if (m_mesh != nullptr)
		m_mesh->draw();

	// Desactivar la textura si la tiene
	if (m_texture != nullptr)
		m_texture->unbind();


	// Dejarlo como estaba
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (m_material.isTranslucid())
	{
		//glDepthMask(GL_TRUE);
	}
}

void Entity::setPosition(glm::dvec3 pos)
{
	// Cambiamos la última columna, que contiene la posición
	modelMat[3][0] = pos.x;
	modelMat[3][1] = pos.y;
	modelMat[3][2] = pos.z;
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
}

void Entity::scale(glm::dvec3 scale)
{
	modelMat = glm::scale(modelMat, scale);
}

void Entity::setMesh(std::string meshID)
{
	m_mesh = (Mesh*) & ResourceManager::Instance()->getMesh(meshID);
}

void Entity::setTexture(std::string textureID)
{
	m_texture = (Texture*)&ResourceManager::Instance()->getTexture(textureID);
}

void Entity::setMaterial(std::string materialID)
{
	m_material = ResourceManager::Instance()->getMaterial(materialID);
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


// - - - - - - - - - - - - - - - - - 

Cubo::Cubo(GLdouble size, bool textured, bool equalFaces)
{
	m_mesh = IndexMesh::generateCube(size, textured, equalFaces);
}


void Cubo::update(GLuint deltaTime)
{
	//rotate(deltaTime * 0.001, { 0,1,0 }, LOCAL);
}


// - - - - - - - - - - - - - - - - - 

Esfera::Esfera(GLdouble size, GLuint subdivisions, bool textured)
{
	m_mesh = IndexMesh::generateSphere(size, subdivisions, textured);
}

void Esfera::update(GLuint timeElapsed)
{
	rotate(timeElapsed * 0.0005, { 0,1,0 }, LOCAL);
}

// - - - - - - - - - - - - - - - - - 

Grid::Grid(GLuint filas, GLuint columnas, GLdouble tamFila, GLdouble tamColumna)
{
	m_mesh = IndexMesh::generateGrid(filas, columnas, tamFila, tamColumna);
}

// - - - - - - - - - - - - - - - - - 

Terrain::Terrain(std::string filename, GLdouble scale)
{
	m_mesh = IndexMesh::generateTerrain(filename, scale);
}

// - - - - - - - - - - - - - - - - - 

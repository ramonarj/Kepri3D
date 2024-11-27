#include "Entity.h"
#include <glut.h>

#include <gtc/type_ptr.hpp>

#include <iostream>

using namespace glm;


void Entity::render()
{
	// Prueba de transformaciones de la matriz de modelado
	// 1) Decirle a OpenGL que la siguiente matriz que cargaremos es de modelado (no de vista/proyección)
	//glMatrixMode(GL_MODELVIEW);

	//// 2) Modificaciones a la matriz de modelado (escalarla x0.5 en este caso)
	//modelMat = glm::scale((mat4)modelMat, glm::vec3(0.5, 2, 0.5));

	//// 3) Cargar la matriz de modelado
	//glLoadMatrixd(value_ptr(modelMat));

	if (m_mesh != nullptr)
		m_mesh->draw();
}

void Entity::update(GLuint timeElapsed)
{
	// Prueba de transformaciones de la matriz de modelado
	// 1) Decirle a OpenGL que la siguiente matriz que cargaremos es de modelado (no de vista/proyección)
	glMatrixMode(GL_MODELVIEW);

	//// 2) Modificaciones a la matriz de modelado
	// Rotaciones
	modelMat = glm::rotate(modelMat, timeElapsed / 500.0, glm::dvec3(0, 0, 1));
	// Traslaciones
	modelMat = glm::translate(modelMat, dvec3(timeElapsed / 1000.0, 0.0, 0.0));

	// Escalado
	//modelMat = glm::scale(modelMat, glm::dvec3(0.99, 0.99, 0.99));

	// Para sacar información de modelMat
	//std::cout << ((double*)(&modelMat))[12] << std::endl;

	//// 3) Cargar la matriz de modelado
	glLoadMatrixd(value_ptr(modelMat));
}


// - - - - - - - - - - - - - - - - - 

EjesRGB::EjesRGB(GLdouble l)
{
	m_mesh = Mesh::generateAxesRGB(l);
}

//void EjesRGB::render()
//{
//	// También podría hacerse con esto
//	/*int windowW = glutGet(GLUT_WINDOW_WIDTH);
//	int windowH = glutGet(GLUT_WINDOW_HEIGHT);*/
//
//	// Obtener información del puerto de vista
//	GLint m_viewport[4];
//	glGetIntegerv(GL_VIEWPORT, m_viewport); // x, y, w, h
//
//	// Cambiarlo a nuestro antojo y pintar la entidad
//	glViewport(0, m_viewport[3] / 2, m_viewport[2] / 2, m_viewport[3] / 2);
//	Entity::render();
//
//	// Volver a dejarlo como estaba
//	glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
//}

// - - - - - - - - - - - - - - - - - 

Poligono::Poligono(GLint sides, bool relleno)
{
	if(relleno)
		m_mesh = Mesh::generateFilledPolygon(sides);
	else
		m_mesh = Mesh::generatePolygon(sides);
}

//void Poligono::render()
//{
//	GLint m_viewport[4];
//	glGetIntegerv(GL_VIEWPORT, m_viewport); 
//
//	glViewport(m_viewport[2] / 2, m_viewport[3] / 2, m_viewport[2] / 2, m_viewport[3] / 2);
//	//glPolygonMode(GL_FRONT, GL_LINE); // -> si quisiéramos que los triángulos no se rellenen, solo las líneas
//
//	Entity::render();
//
//	glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
//	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // el predeterminado
//}

// - - - - - - - - - - - - - - - - - 
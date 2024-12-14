#include "Camera.h"

#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include "Entity.h"

void Viewport::setSize(GLsizei width, GLsizei height)
{
	w = width;
	h = height;
	glViewport(x, y, w, h);
}


void Viewport::setPosition(GLint x, GLint y)
{
	this->x = x;
	this->y = y;
	glViewport(x, y, w, h);
}

// - - - - - - - - - - - - -

void Camera::pitch(GLdouble angle)
{
	modelMat = glm::rotate(modelMat, angle, glm::dvec3(1, 0, 0));
}

void Camera::yaw(GLdouble angle)
{
	modelMat = glm::rotate(modelMat, angle, glm::dvec3(0, 1, 0));
}

void Camera::roll(GLdouble angle)
{
	modelMat = glm::rotate(modelMat, angle, glm::dvec3(0, 0, 1));
}

void Camera::setPosition(glm::dvec3 pos)
{
	// Cambiamos la �ltima columna, que contiene la posici�n
	modelMat[3][0] = pos.x;
	modelMat[3][1] = pos.y;
	modelMat[3][2] = pos.z;
}

// Todo esto est� duplicado con Entity
void Camera::rotate(GLdouble alpha, glm::dvec3 axis, ReferenceSystem refSys)
{
	// Rotaci�n local: postmultiplicamos la matriz de rotaci�n
	if (refSys == LOCAL)
	{
		// por ahora todas las rotaciones son locales
		modelMat = glm::rotate(modelMat, alpha, axis);
	}
	// Rotaci�n global: premultiplicamos la matriz de rotaci�n
	else if (refSys == GLOBAL)
	{
		// Guardarnos la posici�n previa
		glm::dvec3 pos = { modelMat[3][0], modelMat[3][1], modelMat[3][2] };

		// Rotar respecto al eje global
		glm::dmat4 rotMatrix = glm::rotate(glm::dmat4(1.0), alpha, axis);
		modelMat = rotMatrix * modelMat;

		// Devolver la entidad a su sitio
		setPosition(pos);
	}
}

void Camera::moveLR(GLdouble incr)
{
	modelMat = glm::translate(modelMat, glm::dvec3(incr, 0, 0));
}

void Camera::moveUD(GLdouble incr)
{
	modelMat = glm::translate(modelMat, glm::dvec3(0, incr, 0));
}

void Camera::moveFB(GLdouble incr)
{
	modelMat = glm::translate(modelMat, glm::dvec3(0, 0, -incr));
}

void Camera::changePerspective()
{
	orto = !orto;

	glMatrixMode(GL_PROJECTION);
	// Left, Right, Bottom, Top, Near, Far
	// Los valores por defecto son: -1, 1, -1, 1, 1, -1 (est�n al rev�s zNear y zFar)
	// NOTA: usar DOUBLES (con enteros no funciona)
	// Ortogonal
	if(orto)
	{
		//projMat = glm::ortho(-4.0, 4.0, -4.0, 4.0, -100.0, 100.0);
		projMat = glm::ortho(-50.0, 50.0, -50.0, 50.0, 0.0, 500.0);
	}
		
	// Perspectiva
	else
	{
		//projMat = glm::frustum(-1.0, 1.0, -1.0, 1.0, 1.0, -1.0);
		projMat = glm::frustum(-1.0, 1.0, -1.0, 1.0, 1.0, 500.0);
	}

	//PrintMatrix(&projMat);
	glLoadMatrixd(value_ptr(projMat));
	glMatrixMode(GL_MODELVIEW);
}
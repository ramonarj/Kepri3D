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
	// El producto de matrices no es conmutativo; si queremos que la transformación 
	// sea local, es T * R, y global es R * T 
	//auto rotMatrix = glm::rotate((glm::dmat4)(1.0), angle, glm::dvec3(1, 0, 0));
	//PrintMatrix<double, 4>(&rotMatrix);

	//modelMat = rotMatrix * modelMat;
	//PrintMatrix<double, 4>(&modelMat);

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
	modelMat = glm::translate(modelMat, glm::dvec3(0, 0, incr));
}
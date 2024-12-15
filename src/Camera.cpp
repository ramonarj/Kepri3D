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

void Camera::translate(glm::dvec3 transVector, ReferenceSystem refSys)
{
	transVector.z *= -1;
	Entity::translate(transVector, refSys);
}

void Camera::setSize(GLdouble w, GLdouble h)
{
	nearW = w;
	nearH = h;
	updatePM();
}

void Camera::updatePM()
{
	double maxSize = std::max(nearW, nearH);

	// Left, Right, Bottom, Top, Near, Far
	// Los valores por defecto son: -1, 1, -1, 1, 1, -1 (est�n al rev�s zNear y zFar)
	// NOTA: usar DOUBLES (con enteros no funciona)

	// Ortogonal
	if (orto)
	{
		//projMat = glm::ortho(-4.0, 4.0, -4.0, 4.0, -100.0, 100.0);
		projMat = glm::ortho(-nearW / maxSize * 50, nearW / maxSize * 50, 
			-nearH / maxSize * 50, nearH / maxSize * 50, 0.0, 500.0);
	}
	// Perspectiva; teniendo en cuenta el aspect ratio
	else
	{
		//projMat = glm::frustum(-1.0, 1.0, -1.0, 1.0, 1.0, -1.0);
		//projMat = glm::frustum(-1.0, 1.0, -1.0, 1.0, 1.0, 500.0);
		// Cuanto m�s cercano a 0 sea el valor 'near', m�s sensaci�n de velocidad da
		// Cuanto m�s grande (>1), mayor efecto zoom (de apuntado) da
		projMat = glm::frustum(-nearW / maxSize, nearW / maxSize, -nearH / maxSize, nearH / maxSize, 1.0, 500.0);
		// Far > Near > 0
	}

	// Actualizar el tipo de proyecci�n
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(value_ptr(projMat));
	glMatrixMode(GL_MODELVIEW);
}

void Camera::changePerspective()
{
	orto = !orto;
	updatePM();
}
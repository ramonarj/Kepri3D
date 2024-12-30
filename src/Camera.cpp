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

Camera::Camera(Viewport* viewport) : vp(viewport), projMat(1.0), orto(false)
{
	// Volumen de vista por defecto
	nearW = 0;
	nearH = 0;
	nearPlane = 1.0;
	farPlane = 100.0;
	ortoSize = 15.0; // por ejemplo

	setPosition({ 0, 5, 8 });
	//updatePM(); // no es estrictamente necesario
}

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

void Camera::setNearPlane(GLdouble nearPlane)
{
	this->nearPlane = nearPlane;
	updatePM();
}

void Camera::setFarPlane(GLdouble farPlane)
{
	this->farPlane = farPlane;
	updatePM();
}

void Camera::setOrtoSize(GLdouble ortoSize)
{
	this->ortoSize = ortoSize;
	updatePM();
}

void Camera::updatePM()
{
	// Dependerá de si es una resolución apaisada o retrato
	double maxSize = std::max(nearW, nearH);

	// Left, Right, Bottom, Top, Near, Far
	// Los valores por defecto son: -1, 1, -1, 1, 1, -1 (están al revés zNear y zFar)
	// NOTA: usar DOUBLES (con enteros no funciona)

	// Ortogonal
	if (orto)
	{
		projMat = glm::ortho(-nearW / maxSize * ortoSize, nearW / maxSize * ortoSize,
			-nearH / maxSize * ortoSize, nearH / maxSize * ortoSize, nearPlane, farPlane);
		//projMat = glm::ortho(-nearW / maxSize * 50, nearW / maxSize * 50, 
		//	-nearH / maxSize * 50, nearH / maxSize * 50, 0.0, 500.0);
	}
	// Perspectiva; teniendo en cuenta el aspect ratio
	else
	{
		//projMat = glm::frustum(-1.0, 1.0, -1.0, 1.0, 1.0, -1.0);
		//projMat = glm::frustum(-1.0, 1.0, -1.0, 1.0, 1.0, 500.0);
		// Cuanto más cercano a 0 sea el valor 'near', más sensación de velocidad da
		// Cuanto más grande (>1), mayor efecto zoom (de apuntado) da
		projMat = glm::frustum(-nearW / maxSize, nearW / maxSize, -nearH / maxSize, nearH / maxSize, nearPlane, farPlane);
		// Far > Near > 0
	}

	// Actualizar el tipo de proyección
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(value_ptr(projMat));
	glMatrixMode(GL_MODELVIEW);
}

void Camera::changePerspective()
{
	orto = !orto;
	updatePM();
}
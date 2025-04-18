#include "Camera.h"

#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include "Utils.h"
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

void Viewport::update()
{
	glViewport(x, y, w, h);
}

// - - - - - - - - - - - - -

Camera::Camera(Viewport* viewport) : vp(viewport), projMat(1.0), orto(false)
{
	// Volumen de vista por defecto
	fovX = 90;
	fovY = 45; //a.r. = 2:1
	nearPlane = 1.0;
	farPlane = 300.0;
	ortoSize = 35.0; // por ejemplo

	setPosition({ 0, 12, 13 });
	//updatePM(); // no es estrictamente necesario
}

void Camera::translate(glm::dvec3 transVector, ReferenceSystem refSys)
{
	transVector.z *= -1;
	Entity::translate(transVector, refSys);
}

void Camera::lookAt(const glm::dvec3& point, const glm::dvec3& up)
{
	modelMat = glm::inverse(glm::lookAt(glm::dvec3(modelMat[3]), point, up));
}

void Camera::setAspectRatio(GLdouble ar)
{
	this->fovY = fovX / ar;
	updatePM();
}

void Camera::setFOVX(GLdouble fovx)
{
	double ar = fovX / fovY;
	this->fovX = fovx;	
	this->fovY = fovX / ar;
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
	// { Left, Right, Bottom, Top, Near, Far }
	// NOTA: usar DOUBLES (con enteros no funciona)

	// Ortogonal
	if (orto)
	{
		double right = (ortoSize * (fovX / fovY)) / 2.0;
		projMat = glm::ortho(-right, right, -ortoSize / 2, ortoSize / 2, nearPlane, farPlane);
		// Far > Near > 0
	}
	// Perspectiva
	else
	{
		double maxSize = std::max(fovX, fovY);
		// Cuanto más cercano a 0 sea el valor 'near', más sensación de velocidad da
		// Cuanto más grande (>1), mayor efecto zoom (de apuntado) da
		double ar = fovX / fovY;
		projMat = glm::frustum(-fovX / maxSize, fovX / maxSize, -fovY / maxSize, fovY / maxSize, nearPlane, farPlane);
		//projMat = glm::perspective(glm::radians(fovY), ar, nearPlane, farPlane); // con esta si
		//projMat = glm::frustum(-1.0, 1.0, -1.0, 1.0, 1.0, 500.0);
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
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
	// Cambiamos la última columna, que contiene la posición
	modelMat[3][0] = pos.x;
	modelMat[3][1] = pos.y;
	modelMat[3][2] = pos.z;
}

// Todo esto está duplicado con Entity
void Camera::rotate(GLdouble alpha, glm::dvec3 axis, ReferenceSystem refSys)
{
	// Rotación local: postmultiplicamos la matriz de rotación
	if (refSys == LOCAL)
	{
		// por ahora todas las rotaciones son locales
		modelMat = glm::rotate(modelMat, alpha, axis);
	}
	// Rotación global: premultiplicamos la matriz de rotación
	else if (refSys == GLOBAL)
	{
		// Guardarnos la posición previa
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
	// Los valores por defecto son: -1, 1, -1, 1, 1, -1 (están al revés zNear y zFar)
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
		// Cuanto más cercano a 0 sea el valor 'near', más sensación de velocidad da
		// Cuanto más grande (>1), mayor efecto zoom (de apuntado) da
		projMat = glm::frustum(-nearW / maxSize, nearW / maxSize, -nearH / maxSize, nearH / maxSize, 1.0, 500.0);
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
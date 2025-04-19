#include "Camera.h"

#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include "Utils.h"
#include "Entity.h"

Plano::Plano(const glm::vec3& normal, const glm::vec3& point)
{
	this->normal = glm::normalize(normal);
	this->d = glm::dot(-this->normal, point);
}

float Plano::distanceTo(const glm::vec3& point)
{
	// Esto es más rápido
	return glm::dot(normal, point) + d;

	/*
	// Coordenadas homogéneas
	glm::vec4 plano = { normal, d };
	glm::vec4 punto = { point, 1.0 };

	return glm::dot(plano, punto);
	*/
}

// - - - - - - - - - - - - - - - 

void Viewport::setSize(GLsizei width, GLsizei height)
{
	w = width;
	h = height;
	//glViewport(x, y, w, h);
}


void Viewport::setPosition(GLint x, GLint y)
{
	this->x = x;
	this->y = y;
	//glViewport(x, y, w, h);
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

	setPosition({ 0, 10, 10});
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

Frustum Camera::getFrustum()
{ 
	glm::vec3 camPos = getPosition();
	glm::dvec3 nearPlaneCenter = (glm::dvec3)camPos + forward() * nearPlane;
	// Derecha
	glm::vec3 pointInPlane = nearPlaneCenter + right() * (nearPlane * tan(glm::radians(fovX) / 2.0));
	glm::vec3 rightNormal = glm::cross(up(), glm::dvec3(pointInPlane - camPos));
	// Izquierda
	pointInPlane = nearPlaneCenter - right() * (nearPlane * tan(glm::radians(fovX) / 2.0));
	glm::vec3 leftNormal = glm::cross(-up(), glm::dvec3(pointInPlane - camPos));
	// Arriba
	pointInPlane = nearPlaneCenter + up() * (nearPlane * tan(glm::radians(fovY) / 2.0));
	glm::vec3 topNormal = glm::cross(glm::dvec3(pointInPlane - camPos), right());
	// Abajo
	pointInPlane = nearPlaneCenter - up() * (nearPlane * tan(glm::radians(fovY) / 2.0));
	glm::vec3 bottomNormal = glm::cross(glm::dvec3(pointInPlane - camPos), -right());

	Frustum f;
	f.nearPlane = Plano(forward(), nearPlaneCenter);
	f.farPlane = Plano(-forward(), camPos + glm::vec3(forward() * farPlane));
	// Los otros 4 planos convergen en la cámara
	f.rightPlane = Plano(rightNormal, camPos);
	f.leftPlane = Plano(leftNormal, camPos); 
	f.topPlane = Plano(topNormal, camPos);
	f.bottomPlane = Plano(bottomNormal, camPos);
	return f; // TODO actualizar el frustum a la vez que la matriz de perspectiva y no recalcularlo cada frame
}
#ifndef __CAMERA__
#define __CAMERA__

#include <glew.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>

#include "Utils.h"
#include "Entity.h"

//-------------------------------------------------------------------------

class Viewport {
public:
	Viewport(GLsizei width, GLsizei height) : x(0), y(0), w(width), h(height) { };
	~Viewport() { };

	/* Establece las dimensiones del puerto de vista */
	void setSize(GLsizei w, GLsizei h);
	/* Cambia la posición del viewport en la ventana */
	void setPosition(GLint x, GLint y);

	// Getters
	inline GLsizei getX() { return x; };
	inline GLsizei getY() { return y; };
	inline GLsizei getW() { return w; };
	inline GLsizei getH() { return h; };

protected:
	/* Posición del puerto de vista */
	GLint x;
	GLint y;

	/* Dimensiones del puerto de vista */
	GLsizei w;
	GLsizei h;

	void set();
};

//-------------------------------------------------------------------------

class Camera : public Entity
{
public:
	/* Constructora */
	Camera(Viewport* viewport);

	// Heredados
	void render(glm::dmat4 const& viewMat) override {}

	/* Necesario invertir el parámetro Z antes de llamar al método de Entity */
	void translate(glm::dvec3 transVector, ReferenceSystem refSys = GLOBAL);


	// Getters
	/* Devuelve la matriz de vista (inversa de la de modelado) */
	inline glm::dmat4 getViewMat() { return glm::inverse(modelMat); }

	/* Devuelve la matriz de proyección de la escena */
	inline glm::dmat4 getProjMat() { return projMat; }

	/* Devuelve el puerto de vista */
	inline Viewport* getVP() { return vp; }

	/* Devuelve la distancia al plano cercano */
	inline GLdouble getNearPlane() const { return nearPlane; }
	inline GLdouble getFarPlane() const { return farPlane; }
	inline GLdouble getOrtoSize() const { return ortoSize; }

	/* Devuelve 'true' si la cámara usa VV ortográfico (isométrico incluido), false e.o.c. */
	inline bool isOrto() const { return orto; }

	// Setters
	/* Cambia la perspectiva entre ortogonal y frustrum */
	void changePerspective();

	/* Establece las dimensiones de la cámara */
	void setSize(GLdouble aw, GLdouble ah);

	/* Cambia la distancia al plano cercano */
	void setNearPlane(GLdouble nearPlane);
	void setFarPlane(GLdouble farPlane);
	void setOrtoSize(GLdouble ortoSize);

protected:
	//Matriz de la cámara (eye, look, up) -> todavía no se usa
	/* 'Ojo': posición de la cámara */
	//glm::dvec3 eye = { 0.0, 0.0, - 500.0 };
	///* 'Mirada': posición a la que apunta la cámara */
	//glm::dvec3 look = { 0.0, 0.0, 0.0 };
	///* 'Arriba': vector positivo en el eje Y global */
	//glm::dvec3 up = { 0.0, 1.0, 0.0 };

	/* Matriz de modelado de la cámara -> la hereda de Entity
	 La matriz de vista (V) es la inversa de esta matriz */
	//glm::dmat4 modelMat;

	/* Puerto de vista que mostrará lo que hay en el volumen de vista */
	Viewport* vp;

	/* Matriz de proyección de la escena */
	glm::dmat4 projMat;

	/* El tipo de proyección actual (con/sin punto de fuga) */
	bool orto;

	/* Dimensiones del volumen de vista (VV) */
	GLdouble nearPlane;
	GLdouble farPlane;

	GLdouble nearW;
	GLdouble nearH;

	GLdouble ortoSize;

	// Métodos auxiliares
	/* Actualiza la proyección de la escena */
	void updatePM();
};

#endif
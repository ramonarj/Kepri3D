#ifndef __CAMERA__
#define __CAMERA__

#include <freeglut.h>
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
	GLsizei getX() { return x; };
	GLsizei getY() { return y; };
	GLsizei getW() { return w; };
	GLsizei getH() { return h; };

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
	Camera(Viewport* viewport) : vp(viewport), projMat(1.0), orto(true), nearW(0), nearH(0)
	{
		// IMPORTANTE, porque la "cámara" de OpenGL está puesta por defecto en 
		// el (0,0,1) y mirando hacia el (0,0,0), entonces queremos que la 
		// posición y rotación iniciales de nuestra cámara coincidan con esas

		setPosition({ 0, 5, 8 });
		//modelMat = glm::translate(modelMat, glm::dvec3(0, 0, -1));
		//PrintMatrix(&modelMat);
		//modelMat = glm::rotate(modelMat, PI, glm::dvec3(0, 1, 0));
	}

	// Heredados
	void render(glm::dmat4 const& viewMat) override {}

	/* Necesario invertir el parámetro Z antes de llamar al método de Entity */
	void translate(glm::dvec3 transVector, ReferenceSystem refSys = GLOBAL);


	// Getters
	/* Devuelve la matriz de vista (inversa de la de modelado) */
	inline glm::dmat4 getViewMat() { return glm::inverse(modelMat); }

	/* Devuelve el puerto de vista */
	Viewport* getVP() { return vp; }

	// Pitch, yaw y roll han sido eliminados (llamar en su lugar a "rotate" con el eje adecuado y refSys = LOCAL)

	/* Cambia la perspectiva entre ortogonal y frustrum */
	void changePerspective();

	/* Establece las dimensiones de la cámara */
	void setSize(GLdouble aw, GLdouble ah);

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
	GLdouble nearW;
	GLdouble nearH;

	// Métodos auxiliares
	/* Actualiza la proyección de la escena */
	void updatePM();
};

#endif
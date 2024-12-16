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
	/* Cambia la posici�n del viewport en la ventana */
	void setPosition(GLint x, GLint y);

	// Getters
	GLsizei getX() { return x; };
	GLsizei getY() { return y; };
	GLsizei getW() { return w; };
	GLsizei getH() { return h; };

protected:
	/* Posici�n del puerto de vista */
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
		// IMPORTANTE, porque la "c�mara" de OpenGL est� puesta por defecto en 
		// el (0,0,1) y mirando hacia el (0,0,0), entonces queremos que la 
		// posici�n y rotaci�n iniciales de nuestra c�mara coincidan con esas

		setPosition({ 0, 5, 8 });
		//modelMat = glm::translate(modelMat, glm::dvec3(0, 0, -1));
		//PrintMatrix(&modelMat);
		//modelMat = glm::rotate(modelMat, PI, glm::dvec3(0, 1, 0));
	}

	// Heredados
	void render(glm::dmat4 const& viewMat) override {}

	/* Necesario invertir el par�metro Z antes de llamar al m�todo de Entity */
	void translate(glm::dvec3 transVector, ReferenceSystem refSys = GLOBAL);


	// Getters
	/* Devuelve la matriz de vista (inversa de la de modelado) */
	inline glm::dmat4 getViewMat() { return glm::inverse(modelMat); }

	/* Devuelve el puerto de vista */
	Viewport* getVP() { return vp; }

	// Pitch, yaw y roll han sido eliminados (llamar en su lugar a "rotate" con el eje adecuado y refSys = LOCAL)

	/* Cambia la perspectiva entre ortogonal y frustrum */
	void changePerspective();

	/* Establece las dimensiones de la c�mara */
	void setSize(GLdouble aw, GLdouble ah);

protected:
	//Matriz de la c�mara (eye, look, up) -> todav�a no se usa
	/* 'Ojo': posici�n de la c�mara */
	//glm::dvec3 eye = { 0.0, 0.0, - 500.0 };
	///* 'Mirada': posici�n a la que apunta la c�mara */
	//glm::dvec3 look = { 0.0, 0.0, 0.0 };
	///* 'Arriba': vector positivo en el eje Y global */
	//glm::dvec3 up = { 0.0, 1.0, 0.0 };

	/* Matriz de modelado de la c�mara -> la hereda de Entity
	 La matriz de vista (V) es la inversa de esta matriz */
	//glm::dmat4 modelMat;

	/* Puerto de vista que mostrar� lo que hay en el volumen de vista */
	Viewport* vp;

	/* Matriz de proyecci�n de la escena */
	glm::dmat4 projMat;

	/* El tipo de proyecci�n actual (con/sin punto de fuga) */
	bool orto;

	/* Dimensiones del volumen de vista (VV) */
	GLdouble nearW;
	GLdouble nearH;

	// M�todos auxiliares
	/* Actualiza la proyecci�n de la escena */
	void updatePM();
};

#endif
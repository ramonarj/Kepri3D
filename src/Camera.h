#ifndef __CAMERA__
#define __CAMERA__

#include <freeglut.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

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

class Camera
{
public:
	Camera(Viewport* viewport) : vp(viewport), modelMat(1.0) 
	{
		// IMPORTANTE, porque la "c�mara" de OpenGL est� puesta por defecto en 
		// el (0,0,1) y mirando hacia el (0,0,0), entonces queremos que la 
		// posici�n y rotaci�n iniciales de nuestra c�mara coincidan con esas
		//modelMat = glm::translate(modelMat, glm::dvec3(0, 0, -2));
		//modelMat = glm::rotate(modelMat, 3.14, glm::dvec3(0, 1, 0));
	}

	/* Devuelve el puerto de vista */
	Viewport* getVP() { return vp; }

	/* Cambia la posici�n de la c�mara */
	inline void SetPosition(glm::dvec3 pos) { eye = pos; }

	/* Getters*/
	inline glm::dmat4 getViewMat() { return glm::inverse(modelMat); }

	// Giros en el eje de coordenadas local
	/* Rota la c�mara alrededor del eje X local */
	void pitch(GLdouble angle);
	/* Rota la c�mara alrededor del eje Y local */
	void yaw(GLdouble angle);
	/* Rota la c�mara alrededor del eje Z local */
	void roll(GLdouble angle);

	// Movimientos en el eje de coordenadas local 
	/* Mueve la c�mara hacia izquierda/derecha en su eje X local */
	void moveLR(GLdouble incr);
	/* Mueve la c�mara hacia arriba/abajo en su eje Y local */
	void moveUD(GLdouble incr);
	/* Mueve la c�mara hacia adelante/atr�s en su eje Z local */
	void moveFB(GLdouble incr);

protected:
	//Matriz de la c�mara (eye, look, up) -> todav�a no se usa
	/* 'Ojo': posici�n de la c�mara */
	glm::dvec3 eye = { 0.0, 0.0, - 500.0 };
	/* 'Mirada': posici�n a la que apunta la c�mara */
	glm::dvec3 look = { 0.0, 0.0, 0.0 };
	/* 'Arriba': vector positivo en el eje Y global */
	glm::dvec3 up = { 0.0, 1.0, 0.0 };

	/* Matriz de modelado de la c�mara
	 La matriz de vista es la inversa de esta matriz */
	glm::dmat4 modelMat;

	/* Puerto de vista que mostrar� lo que hay en el volumen de vista */
	Viewport* vp;
};

#endif
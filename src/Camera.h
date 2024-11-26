#ifndef __CAMERA__
#define __CAMERA__

#include <freeglut.h>
#include <glm.hpp>

//-------------------------------------------------------------------------

class Viewport {
public:
	Viewport(GLsizei width, GLsizei height) : x(0), y(0), w(w), h(h) { };
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
	Camera(Viewport* viewport) : vp(viewport) {}

	/* Cambia la posici�n de la c�mara */
	inline void SetPosition(glm::dvec3 pos) { eye = pos; }

	/* Cambia el objetivo de la c�mara */
	inline void LookAt(glm::dvec3 pos) { look = pos; }
protected:
	//Matriz de la c�mara (eye, look, up)
	/* 'Ojo': posici�n de la c�mara */
	glm::dvec3 eye = { 0.0, 0.0, - 500.0 };
	/* 'Mirada': posici�n a la que apunta la c�mara */
	glm::dvec3 look = { 0.0, 0.0, 0.0 };
	/* 'Arriba': vector positivo en el eje Y global */
	glm::dvec3 up = { 0.0, 1.0, 0.0 };

	/* Puerto de vista que mostrar� lo que hay en el volumen de vista */
	Viewport* vp;
};

#endif
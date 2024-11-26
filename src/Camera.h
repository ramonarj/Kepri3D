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

class Camera
{
public:
	Camera(Viewport* viewport) : vp(viewport) {}

	/* Cambia la posición de la cámara */
	inline void SetPosition(glm::dvec3 pos) { eye = pos; }

	/* Cambia el objetivo de la cámara */
	inline void LookAt(glm::dvec3 pos) { look = pos; }
protected:
	//Matriz de la cámara (eye, look, up)
	/* 'Ojo': posición de la cámara */
	glm::dvec3 eye = { 0.0, 0.0, - 500.0 };
	/* 'Mirada': posición a la que apunta la cámara */
	glm::dvec3 look = { 0.0, 0.0, 0.0 };
	/* 'Arriba': vector positivo en el eje Y global */
	glm::dvec3 up = { 0.0, 1.0, 0.0 };

	/* Puerto de vista que mostrará lo que hay en el volumen de vista */
	Viewport* vp;
};

#endif
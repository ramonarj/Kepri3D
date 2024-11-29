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
	Camera(Viewport* viewport) : vp(viewport), modelMat(1.0) 
	{
		// IMPORTANTE, porque la "cámara" de OpenGL está puesta por defecto en 
		// el (0,0,1) y mirando hacia el (0,0,0), entonces queremos que la 
		// posición y rotación iniciales de nuestra cámara coincidan con esas
		//modelMat = glm::translate(modelMat, glm::dvec3(0, 0, -2));
		//modelMat = glm::rotate(modelMat, 3.14, glm::dvec3(0, 1, 0));
	}

	/* Devuelve el puerto de vista */
	Viewport* getVP() { return vp; }

	/* Cambia la posición de la cámara */
	inline void SetPosition(glm::dvec3 pos) { eye = pos; }

	/* Getters*/
	inline glm::dmat4 getViewMat() { return glm::inverse(modelMat); }

	// Giros en el eje de coordenadas local
	/* Rota la cámara alrededor del eje X local */
	void pitch(GLdouble angle);
	/* Rota la cámara alrededor del eje Y local */
	void yaw(GLdouble angle);
	/* Rota la cámara alrededor del eje Z local */
	void roll(GLdouble angle);

	// Movimientos en el eje de coordenadas local 
	/* Mueve la cámara hacia izquierda/derecha en su eje X local */
	void moveLR(GLdouble incr);
	/* Mueve la cámara hacia arriba/abajo en su eje Y local */
	void moveUD(GLdouble incr);
	/* Mueve la cámara hacia adelante/atrás en su eje Z local */
	void moveFB(GLdouble incr);

protected:
	//Matriz de la cámara (eye, look, up) -> todavía no se usa
	/* 'Ojo': posición de la cámara */
	glm::dvec3 eye = { 0.0, 0.0, - 500.0 };
	/* 'Mirada': posición a la que apunta la cámara */
	glm::dvec3 look = { 0.0, 0.0, 0.0 };
	/* 'Arriba': vector positivo en el eje Y global */
	glm::dvec3 up = { 0.0, 1.0, 0.0 };

	/* Matriz de modelado de la cámara
	 La matriz de vista es la inversa de esta matriz */
	glm::dmat4 modelMat;

	/* Puerto de vista que mostrará lo que hay en el volumen de vista */
	Viewport* vp;
};

#endif
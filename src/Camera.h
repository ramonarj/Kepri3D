#ifndef __CAMERA__
#define __CAMERA__

#include <freeglut.h>
#include <glm.hpp>

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
	Camera(Viewport* viewport) : vp(viewport), modelMat(1.0) {}

	/* Devuelve el puerto de vista */
	Viewport* getVP() { return vp; }

	/* Cambia la posición de la cámara */
	inline void SetPosition(glm::dvec3 pos) { eye = pos; }

	/* Getters*/
	inline glm::dmat4 getViewMat() { return glm::inverse(modelMat); }
	glm::dmat4& getModelMat() { return modelMat; }

	/* Setters */
	void setModelMat(const glm::dmat4& mat) { modelMat = mat; }

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
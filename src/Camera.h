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
	/* Cambia la posici�n del viewport en la ventana */
	void setPosition(GLint x, GLint y);

	// Getters
	inline GLsizei getX() { return x; };
	inline GLsizei getY() { return y; };
	inline GLsizei getW() { return w; };
	inline GLsizei getH() { return h; };

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
	Camera(Viewport* viewport);

	// Heredados
	void render(glm::dmat4 const& viewMat) override {}

	/* Necesario invertir el par�metro Z antes de llamar al m�todo de Entity */
	void translate(glm::dvec3 transVector, ReferenceSystem refSys = GLOBAL);


	// Getters
	/* Devuelve la matriz de vista (inversa de la de modelado) */
	inline glm::dmat4 getViewMat() { return glm::inverse(modelMat); }

	/* Devuelve la matriz de proyecci�n de la escena */
	inline glm::dmat4 getProjMat() { return projMat; }

	/* Devuelve el puerto de vista */
	inline Viewport* getVP() { return vp; }

	/* Devuelve la distancia al plano cercano */
	inline GLdouble getNearPlane() const { return nearPlane; }
	inline GLdouble getFarPlane() const { return farPlane; }
	inline GLdouble getOrtoSize() const { return ortoSize; }

	/* Devuelve 'true' si la c�mara usa VV ortogr�fico (isom�trico incluido), false e.o.c. */
	inline bool isOrto() const { return orto; }

	// Setters
	/* Cambia la perspectiva entre ortogonal y frustrum */
	void changePerspective();

	/* Establece las dimensiones de la c�mara */
	void setSize(GLdouble aw, GLdouble ah);

	/* Cambia la distancia al plano cercano */
	void setNearPlane(GLdouble nearPlane);
	void setFarPlane(GLdouble farPlane);
	void setOrtoSize(GLdouble ortoSize);

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
	GLdouble nearPlane;
	GLdouble farPlane;

	GLdouble nearW;
	GLdouble nearH;

	GLdouble ortoSize;

	// M�todos auxiliares
	/* Actualiza la proyecci�n de la escena */
	void updatePM();
};

#endif
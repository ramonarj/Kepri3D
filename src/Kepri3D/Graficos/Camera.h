#ifndef __CAMERA__
#define __CAMERA__

#include <glew.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>

#include "Entity.h"

struct Plano
{
	Plano(){}
	Plano(const glm::vec3& normal, const glm::vec3& point);
	float distanceTo(const glm::vec3& point);

	// Vector unitario con la normal del plano
	glm::vec3 normal;
	// Distancia m�nima al origen de coordenadas
	float d;
};

struct Frustum
{
	Plano nearPlane;
	Plano farPlane;
	Plano leftPlane;
	Plano rightPlane;
	Plano topPlane;
	Plano bottomPlane;
};

//-------------------------------------------------------------------------

class Viewport {
public:
	Viewport(GLsizei width, GLsizei height) : x(0), y(0), w(width), h(height) { };
	~Viewport() { };

	/* Establece las dimensiones del puerto de vista */
	void setSize(GLsizei w, GLsizei h);
	/* Cambia la posici�n del viewport en la ventana */
	void setPosition(GLint x, GLint y);

	/* Llama a glViewport con la informaci�n de este viewport */
	void update();

	// Getters
	inline GLsizei getX() const { return x; };
	inline GLsizei getY() const { return y; };
	inline GLsizei getW() const { return w; };
	inline GLsizei getH() const{ return h; };

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
	inline glm::dvec3 forward() { return -modelMat[2]; }

	/* Orienta la c�mara hacia el punto dado, en coordenadas globales */
	void lookAt(const glm::dvec3& point, const glm::dvec3& up = { 0, 1, 0 });

	/* Cambia la perspectiva entre ortogonal y frustrum */
	void changePerspective();

	// Getters
	/* Devuelve la matriz de vista (inversa de la de modelado) */
	inline const glm::dmat4& getViewMat() { return glm::inverse(modelMat); }

	inline const glm::dmat4& getProjMat() { return projMat; }
	inline Viewport* getVP() { return vp; }
	inline GLdouble getNearPlane() const { return nearPlane; }
	inline GLdouble getFarPlane() const { return farPlane; }
	inline GLdouble getOrtoSize() const { return ortoSize; }

	/* Devuelve 'true' si la c�mara usa VV ortogr�fico (isom�trico incluido), false e.o.c. */
	inline bool isOrto() const { return orto; }

	/* Devuelve el volumen de vista de la c�mara, compuesto por los 6 planos */
	Frustum getFrustum();

	// Setters
	void setAspectRatio(GLdouble ar);
	void setFOVX(GLdouble fovx);
	void setNearPlane(GLdouble nearPlane);
	void setFarPlane(GLdouble farPlane);

	void setOrtoSize(GLdouble ortoSize);

	// Est�ticos
	/* Cambia el color de fondo de la c�mara */
	inline static void setBackgroundColor(float r, float g, float b) { glClearColor(r, g, b, 1.0); }

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

	GLdouble fovX;
	GLdouble fovY;

	GLdouble ortoSize;

	// M�todos auxiliares
	/* Actualiza la proyecci�n de la escena */
	void updatePM();
};

#endif
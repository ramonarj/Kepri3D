#ifndef __ENTITY__
#define __ENTITY__

#include "Mesh.h"
#include <glm.hpp>
#include <freeglut.h>

class Entity
{
public:
	/* Constructora por defecto */
	Entity() : modelMat(1.0) {} // Pone la matriz de modelado a la matriz identidad de grado 4 (1 0 0 0 / 0 1 0 0 ...)
	
	
	virtual ~Entity() { };

	virtual void render();
	virtual void update(GLuint timeElapsed);

protected:
	/* Malla/s que usará la entidad para pintarse */
	Mesh* m_mesh;

	/* Matriz de modelado de la entidad; recoge las transformaciones 
	(traslaciones, rotaciones, escalados) hechas a la entidad */
	glm::dmat4 modelMat;

	//Mesh* mesh = nullptr;
	//Mesh* mesh2 = nullptr;
	//Texture texture;
	//Texture texture2;


	//virtual void draw();
	//virtual void setMvM(glm::dmat4 const& modelViewMat);

	////Método auxiliar
	//virtual void drawTexture(GLenum face, Mesh* meshUsed, Texture* textureUsed);
};

// - - - - - - - - - - - -

class EjesRGB : public Entity
{
public:
	EjesRGB(GLdouble l);
	~EjesRGB() { };
	//void render() override;
};

// - - - - - - - - - - - - 

class Poligono : public Entity
{
public:
	Poligono(GLint sides, bool relleno);
	~Poligono() { };
	//void render() override;
};

#endif
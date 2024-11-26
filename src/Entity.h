#ifndef __ENTITY__
#define __ENTITY__

#include "Mesh.h"
#include <glm.hpp>
#include <freeglut.h>

class Entity
{
public:
	Entity();
	virtual ~Entity() { };

	virtual void render();
	//virtual void update(GLuint timeElapsed) {};

protected:
	Mesh* m_mesh;

	//Mesh* mesh = nullptr;
	//Mesh* mesh2 = nullptr;
	//Texture texture;
	//Texture texture2;
	//glm::dmat4 modelMat;

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
	//virtual void render();
};

// - - - - - - - - - - - - 

class Poligono : public Entity
{
public:
	Poligono(GLint sides, bool relleno);
	~Poligono() { };
	//virtual void render();
};

#endif
#ifndef __MATERIAL__
#define __MATERIAL__

#include <glm.hpp>
#include <freeglut.h>

class Material
{
public: 
	/* Constructora por defecto */
	Material();
	Material(glm::fvec4 ambient, glm::fvec4 diffuse, glm::fvec4 specular, GLfloat expF = 0);

	/* Carga el material */
	virtual void load();
private:
	/* Componente ambiente del material */
	glm::fvec4 m_ambient;
	/* Componente difusa del material */
	glm::fvec4 m_diffuse;
	/* Componente especular del material */
	glm::fvec4 m_specular;

	/* Exponente especular */
	GLfloat m_expF;

	/* ¿Emite luz? */
	//bool m_lightEmitter;
};

#endif
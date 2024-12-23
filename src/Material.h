#ifndef __MATERIAL__
#define __MATERIAL__

#include <glm.hpp>
#include <glew.h>

class Material
{
public: 
	/* Constructora por defecto */
	Material();
	Material(glm::fvec4 ambient, glm::fvec4 diffuse, glm::fvec4 specular, GLfloat expF = 0);

	/* Carga el material */
	virtual void load();

	static inline void setShadingType(GLuint sh) { m_shading = sh; }

	/* Indica si el material es translúcido o no */
	inline bool isTranslucid() const { return m_translucent; }
private:
	/* Componente ambiente del material */
	glm::fvec4 m_ambient;
	/* Componente difusa del material */
	glm::fvec4 m_diffuse;
	/* Componente especular del material */
	glm::fvec4 m_specular;

	/* Exponente especular { 0 - 128 } */
	GLfloat m_brillo;

	/* Cara en la que se usará el material */
	GLuint m_face;

	/* Tipo de sombreado para el material */
	static GLuint m_shading;

	/* ¿Es translúcido? */
	bool m_translucent;

	/* ¿Emite luz? */
	//bool m_lightEmitter;
};

#endif
#ifndef __MATERIAL__
#define __MATERIAL__

#include <glm.hpp>
#include <glew.h>

class Material
{
public: 
	/* Constructora por defecto */
	Material();
	Material(glm::fvec4 ambient, glm::fvec4 diffuse, glm::fvec4 specular,
		glm::fvec4 emission = { 0, 0, 0, 1 }, GLfloat brillo = 0);

	/* Carga el material */
	virtual void load();

	// Setters
	/* Indica el tipo de sombreado que se usar� para TODOS los materiales */
	static inline void setShadingType(GLuint sh) { m_shading = sh; }

	// Getters
	/* Indica si el material es transl�cido o no */
	inline bool isTranslucid() const { return m_translucent; }

	/* Devuelve la componente ambiente del material */
	inline const glm::vec3& getAmbient() const { return m_ambient; }

	/* Devuelve la componente difusa del material */
	inline const glm::vec3& getDiffuse() const { return m_diffuse; }

	/* Devuelve la componente especular del material */
	inline const glm::vec3& getSpecular() const { return m_specular; }

	/* Devuelve el brillo especular del material (0 - 128)*/
	inline GLfloat getBrillo() const { return m_brillo; }
private:
	/* Componente ambiente del material */
	glm::fvec4 m_ambient;
	/* Componente difusa del material */
	glm::fvec4 m_diffuse;
	/* Componente especular del material */
	glm::fvec4 m_specular;

	/* Exponente especular { 0 - 128 } */
	GLfloat m_brillo;

	/* Para materiales que 'emiten' luz (o m�s bien, que se ven cuando no hay luz) */
	glm::fvec4 m_emission;

	/* Cara en la que se usar� el material */
	GLuint m_face;

	/* Tipo de sombreado para el material */
	static GLuint m_shading;

	/* �Es transl�cido? */
	bool m_translucent;

	/* �Emite luz? */
	//bool m_lightEmitter;
};

#endif
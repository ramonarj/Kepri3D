#ifndef __MATERIAL__
#define __MATERIAL__

#include <glm.hpp>
#include <glew.h>

class Shader;
class Texture;

const unsigned int NUM_TEXTURES = 7;

class Material
{
public: 
	/* Constructora por defecto */
	Material();
	Material(glm::fvec4 ambient, glm::fvec4 diffuse, glm::fvec4 specular,
		glm::fvec4 emission = { 0, 0, 0, 1 }, GLfloat brillo = 0);

	/* Carga el material en la Fixed Pipeline */
	virtual void load();

	// Lo mismo pero con shaders
	void loadToShader(Shader* sh);
	/* Limpia los atributos */
	void unload();

	inline void setTexture(unsigned int i, Texture* tex) { m_textures[i] = tex; }

	// Setters
	/* Indica el tipo de sombreado que se usará para TODOS los materiales */
	static inline void setShadingType(GLuint sh) { m_shading = sh; }

	// Getters
	/* Devuelve la componente ambiente del material */
	inline const glm::vec3& getAmbient() const { return m_ambient; }

	/* Devuelve la componente difusa del material */
	inline const glm::vec4& getDiffuse() const { return m_diffuse; }

	/* Devuelve la componente especular del material */
	inline const glm::vec3& getSpecular() const { return m_specular; }

	/* Devuelve el brillo especular del material (0 - 128)*/
	inline GLfloat getBrillo() const { return m_brillo; }

	inline const Texture* getTexture(GLuint i) const { return m_textures[i]; }

	static bool fresnel;
private:
	/* Componentes del material */
	glm::fvec4 m_ambient;
	glm::fvec4 m_diffuse;
	glm::fvec4 m_specular;

	/* Exponente especular { 0 - 128 } */
	GLfloat m_brillo;

	/* Para materiales que 'emiten' luz (o más bien, que se ven cuando no hay luz) */
	glm::fvec4 m_emission;

	/* Cara en la que se usará el material */
	GLuint m_face;

	/* Tipo de sombreado para los materiales (compartido) */
	static GLuint m_shading;

	/* Texturas utilizadas por el material*/
	// 0 = Textura Principal
	// 1 = Textura Secundaria
	// 2 = Specular Map
	// 3 = Normal Map
	// 4 = Displacement Map
	Texture* m_textures[NUM_TEXTURES];

	// Métodos auxiliares
	/* Manda las texturas al shader */
	void bindTextures(Shader* sh);
};

#endif
#ifndef __MATERIAL__
#define __MATERIAL__

#include <glm.hpp>
#include <glew.h>
#include <map>
#include <string>

class Shader;
class Texture;

const unsigned int NUM_TEXTURES = 8;

class Material
{
public: 
	/* Constructora por defecto */
	Material();
	Material(glm::fvec4 ambient, glm::fvec4 diffuse, glm::fvec4 specular,
		glm::fvec4 emission = { 0, 0, 0, 1 }, GLfloat brillo = 0);

	/* Carga el material en la Fixed Pipeline */
	virtual void load();

	/* Carga el material al shader (lo mismo pero con shaders) */
	void loadToShader(Shader* sh);

	void setTexture(unsigned int i, Texture* tex);

	// Setters
	/* Indica el tipo de sombreado que se usará para TODOS los materiales */
	static inline void setShadingType(GLuint sh) { m_shading = sh; }

	void setShader(Shader* sh) { m_shader = sh; }

	void setEmission(const glm::vec3& emission) { m_emission = glm::vec4(emission, 1.0); }

	// Getters
	Shader* getShader() const { return m_shader; }

	/* Devuelve la componente ambiente del material */
	inline const glm::vec3& getAmbient() const { return m_ambient; }

	/* Devuelve la componente difusa del material */
	inline const glm::vec4& getDiffuse() const { return m_diffuse; }

	/* Devuelve la componente especular del material */
	inline const glm::vec3& getSpecular() const { return m_specular; }

	/* Devuelve el brillo especular del material (0 - 128)*/
	inline GLfloat getBrillo() const { return m_brillo; }

	inline const Texture* getTexture(GLuint i) const { return m_textures[i]; }

	inline bool needsTangents() { return m_needsTangents; }

	/* - - Uniforms - - */
	void setFloat(const std::string& name, float value);
	void setInt(const std::string& name, int value);
	void setVec2(const std::string& name, glm::vec2 value);
	void setVec3(const std::string& name, glm::vec3 value);
	void setTexture(const std::string& name, Texture* value);

	// Getters
	glm::vec3 getVec3(const std::string& name);

	static bool fresnel;
	static bool s_useTextures;
private:
	/* Shader usado por el material */
	Shader* m_shader;

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
	// 5 = Reflection Map
	// 6 = Skybox
	// 7 = Emission Map
	Texture* m_textures[NUM_TEXTURES];

	/* - - Uniforms - - */
	std::map<std::string, float> m_floatUniforms;
	std::map<std::string, int> m_intUniforms;
	std::map<std::string, glm::vec2> m_vec2Uniforms;
	std::map<std::string, glm::vec3> m_vec3Uniforms;
	std::map<std::string, Texture*> m_texUniforms;

	// Métodos auxiliares
	/* Manda las texturas al shader */
	void bindTextures(Shader* sh);
	void sendCustomUniforms(Shader* sh);

	/* Necesita que la malla provea vectores tangentes/no */
	bool m_needsTangents;
};

#endif
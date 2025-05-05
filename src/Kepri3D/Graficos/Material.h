#ifndef __MATERIAL__
#define __MATERIAL__

#include <glm.hpp>
#include <glew.h>
#include <map>
#include <string>

class Shader;
class Texture;

const unsigned int NUM_TEXTURES = 8;

// Texturas principales usadas por el material, que siempre se pasan al shader
#define DIFFUSE_MAP 0
#define SECONDARY_MAP 1
#define SPECULAR_MAP 2
#define NORMAL_MAP 3
#define DISPLACEMENT_MAP 4
#define REFLECTION_MAP 5
#define SKYBOX_MAP 6
#define EMISSION_MAP 7

class Material
{
public: 
#define estate 2
	static const glm::vec4 DEFAULT_AMBIENT;
	static const glm::vec4 DEFAULT_DIFFUSE;
	static const glm::vec4 DEFAULT_SPECULAR;
	static const glm::vec4 DEFAULT_EMISSION;
	static const float DEFAULT_BRILLO;

	/* Constructora por defecto */
	Material();
	Material(glm::fvec4 ambient, glm::fvec4 diffuse, glm::fvec4 specular,
		glm::fvec4 emission = DEFAULT_EMISSION, GLfloat brillo = 0);

	/* Carga el material en la Fixed Pipeline */
	virtual void load();

	/* Carga el material al shader (lo mismo pero con shaders) */
	void loadToShader(Shader* sh);

	// Setters
	inline void setShader(Shader* sh) { m_shader = sh; }
	inline void setEmission(const glm::vec3& emission) { m_emission = glm::vec4(emission, 1.0); }
	void setTexture(unsigned int i, Texture* tex);

	// Getters
	inline Shader* getShader() const { return m_shader; }
	inline const glm::vec3& getAmbient() const { return m_ambient; }
	inline const glm::vec4& getDiffuse() const { return m_diffuse; }
	inline const glm::vec3& getSpecular() const { return m_specular; }
	inline const glm::vec4& getEmission() const { return m_emission; }
	inline float getBrillo() const { return m_brillo; } // (0 - 128)
	const Texture* getTexture(GLuint i) const;
	inline bool needsTangents() { return m_needsTangents; }

	/* - - - - - Uniforms - - - - - */
	// Setters
	void setFloat(const std::string& name, float value);
	void setInt(const std::string& name, int value);
	void setVec2(const std::string& name, glm::vec2 value);
	void setVec3(const std::string& name, glm::vec3 value);
	void setTexture(const std::string& name, Texture* value);

	// Getters
	glm::vec2 getVec2(const std::string& name);
	glm::vec3 getVec3(const std::string& name);


	/* - - - - Estaticos - - - - */
	static inline void setShadingType(GLuint sh) { m_shading = sh; }

	static bool fresnel;
	static bool s_useTextures;
private:
	/* Tipo de sombreado para los materiales (compartido) */
	static GLuint m_shading;

	/* Shader usado por el material */
	Shader* m_shader;

	/* Componentes del material */
	glm::fvec4 m_ambient;
	glm::fvec4 m_diffuse;
	glm::fvec4 m_specular;

	/* Exponente especular { 0 - 128 } */
	float m_brillo;

	/* Para materiales que 'emiten' luz (o más bien, que se ven cuando no hay luz) */
	glm::fvec4 m_emission;

	/* Cara en la que se usará el material */
	GLuint m_face;

	/* Necesita que la malla provea vectores tangentes/no */
	bool m_needsTangents;

	/* Texturas principales del material */
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
};

#endif
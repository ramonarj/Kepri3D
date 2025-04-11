#ifndef __KEPRI_SHADER__
#define __KEPRI_SHADER__

#include <glew.h>
#include <string>
#include <glm.hpp>

#include "Defines.h"

class Texture;

class Shader
{
public:
	Shader();

	/* Carga un shader específico para ser usado por el programa */
	std::string preprocess(const char* shaderSrc);

	/* Carga un shader específico para ser usado por el programa */
	void load(GLenum shaderType, const char* shaderSrc);

	/* Enlaza el shader program usando todos los shaders cargados */
	void link();

	/* Activa/carga este shader */
	inline void use() const 
	{
		// Evitar llamadas innecesarias a 'glUseProgram' (da fallos)
		if (s_activeProgram == programId) { return; }

		glUseProgram(programId);
		s_activeProgram = programId;
#ifdef __DEBUG_INFO__
		programChanges++;
#endif
	}

	/* Desactiva el uso de shaders, eliminando el programa que hubiera cargado (si lo había) */
	static inline void turnOff() 
	{
		if (s_activeProgram == 0) { return; }

		glUseProgram(0); 
		s_activeProgram = 0;
#ifdef __DEBUG_INFO__
		programChanges++;
#endif
	}

	// - - - Paso de uniforms al shader - - - // 
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec2(const std::string& name, const glm::vec2 value) const;
	void setVec3(const std::string& name, const glm::vec3 value) const; 
	void setVec4(const std::string& name, const glm::vec4 value) const;
	void setMat4(const std::string& name, const glm::mat4 matValue) const;
	void setMat4d(const std::string& name, const glm::dmat4 value) const; // TODO por referencia

	void setTexture(const std::string& name, int index, Texture* tex) const;

	// - - - Obtención de uniforms del shader - - - //
	glm::dmat4 getMat4d(const std::string& name) const;

	// - - - Configurar los Uniform blocks - - - //
	void bindUniformBlock(const std::string& name, unsigned int bindingPoint);

	// - - - Otros - - - //
	void useTextures(bool b) { m_useTextures = b; }
	bool useTextures() const { return m_useTextures; }

#ifdef __DEBUG_INFO__
	static unsigned int programChanges;
#endif
private:
	/* Shader activo actualmente */
	static unsigned int s_activeProgram;

	/* IDs del Vertex, Tess. Control, Tess. Eval., Geometry y Fragment shader (en ese orden) */
	unsigned int shadersIds[5];

	/* ID del shader program */
	unsigned int programId;

	// Para optimizar el paso de uniforms
	bool m_useTextures = true;
};

#endif
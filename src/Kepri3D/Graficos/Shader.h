#ifndef __KEPRI_SHADER__
#define __KEPRI_SHADER__

#include <glew.h>
#include <string>
#include <glm.hpp>

#include "Game.h"

class Shader
{
public:
	Shader();

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

	// - - - Paso de uniforms - - - // 
	/* Pasa un uniform de tipo INT al shader */
	void setInt(const std::string& name, int value) const;

	/* Pasa un uniform de tipo FLOAT al shader */
	void setFloat(const std::string& name, float value) const;

	/* Pasa un uniform de tipo VEC2 al shader */
	void setVec2(const std::string& name, const glm::vec2 value) const;

	/* Pasa un uniform de tipo VEC3 al shader */
	void setVec3(const std::string& name, const glm::vec3 value) const; 

	/* Pasa un uniform de tipo VEC4 al shader */
	void setVec4(const std::string& name, const glm::vec4 value) const;

	/* Pasa un uniform de tipo MAT4 al shader */
	void setMat4(const std::string& name, const glm::mat4 matValue) const;

	/* Pasa un uniform de tipo DMAT4 al shader */
	void setMat4d(const std::string& name, const glm::dmat4 value) const; // TODO por referencia

	// - - - Configurar los Uniform blocks - - - //
	void bindUniformBlock(const std::string& name, unsigned int bindingPoint);


#ifdef __DEBUG_INFO__
	static unsigned int programChanges;
#endif
private:
	/* IDs del Vertex, Tess. Control, Tess. Eval., Geometry y Fragment shader (en ese orden) */
	unsigned int shadersIds[5];

	/* ID del shader program */
	unsigned int programId;

	/* Shader activo actualmente */
	static unsigned int s_activeProgram;
};

#endif
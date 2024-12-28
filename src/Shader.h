#ifndef __KEPRI_SHADER__
#define __KEPRI_SHADER__

#include <glew.h>

class Shader
{
public:
	Shader();

	/* Carga un shader específico para ser usado por el programa */
	void load(GLenum shaderType, const char* shaderSrc);

	/* Enlaza el shader program usando todos los shaders cargados */
	void link();

	inline unsigned int getId() const { return programId; }
private:
	/* IDs del Vertex, geometry y fragment shader (de momento) */
	unsigned int shadersIds[3];

	/* ID del shader program */
	unsigned int programId;
};

#endif
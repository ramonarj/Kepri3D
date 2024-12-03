#ifndef __TEXTURE__
#define __TEXTURE__

#include <freeglut.h>
#include <string>

const std::string TEXTURES_PATH = "..\\bin\\assets\\";

class Texture
{
public:
	/* Constructora por defecto */
	Texture() : w(0), h(0), id(0) {}
	/* Borra la textura asociada si ten�a alguna */
	~Texture() { if (id != 0) glDeleteTextures(1, &id); };

	/* Carga la textura */
	bool load(const std::string& bmpName, GLubyte alpha = 255);

	// Activar/desactivar la textura
	void bind(GLuint mix = GL_REPLACE);
	void unbind();
private:
	// Dimensiones de la textura (en p�xeles)
	GLuint w;
	GLuint h;

	/* Identificador interno (GPU) de la textura */
	GLuint id;

	// M�todos privados
	/* Inicializa la textura si no se hab�a creado ya */
	void Init();
};

#endif
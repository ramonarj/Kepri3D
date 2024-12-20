#ifndef __TEXTURE__
#define __TEXTURE__

#include <freeglut.h>
#include <string>
#include <gl/GLext.h>


class Texture
{
public:
	/* Constructora por defecto */
	Texture() : w(0), h(0), id(0) {}
	/* Borra la textura asociada si tenía alguna */
	~Texture() { if (id != 0) glDeleteTextures(1, &id); };

	/* Carga la textura */
	bool load(const std::string& filePath, GLubyte alpha = 255);

	// Activar/desactivar la textura
	/* Activa la textura y mezcla la luz de la escena con el mix dado
	->REPLACE: no se tienen en cuenta las luces
	->MODULATE: luz * textura
	->ADD: luz + textura (efecto quemado) */
	void bind(GLuint mix = GL_MODULATE);
	void unbind();

	/* Carga un rectángulo de píxeles RGBA de tamaño 'width * height' desde el buffer 'buf' para su posterior lectura */
	static void loadColorBuffer(GLsizei width, GLsizei height, GLenum buf = GL_BACK);

	/* Salva la textura obtenida del color buffer en un archivo BMP */
	static void save(const std::string& BMP_Name);

	//Getters
	/* Ancho de la textura, en píxeles */
	inline GLuint getWidth() const{ return w; };

	/* Alto de la textura, en píxeles */
	inline GLuint getHeight() const { return h; };

private:
	// Dimensiones de la textura (en píxeles)
	GLuint w;
	GLuint h;

	/* Identificador interno (GPU) de la textura */
	GLuint id;

	// Métodos privados
	/* Inicializa la textura si no se había creado ya */
	void Init();
};

#endif
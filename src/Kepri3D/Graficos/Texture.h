#ifndef __TEXTURE__
#define __TEXTURE__

#include <glew.h>
#include <string>
#include <vector>
#include <glm.hpp>
#include "Game.h"

// GL_RGBA -> sin correci�n gamma
// GL_SRGB_ALPHA -> con correcci�n gamma
const GLint COLOR_SPACE = GL_SRGB_ALPHA;

class Texture
{
public:
	/* Constructora por defecto */
	Texture() : w(0), h(0), id(0), hasMipmap(false), m_hasAlpha(false) {}
	/* Borra la textura asociada si ten�a alguna */
	virtual ~Texture() { if (id != 0) glDeleteTextures(1, &id); };

	/* Carga la textura y le da la transparencia indicada. Usa la librer�a STBI */
	bool load(const std::string& filePath, GLubyte alpha = 255, GLint intFormat = COLOR_SPACE);
	/* Carga la textura con la librer�a PixMap32RGBA */
	bool load(const std::string& filePath, const glm::ivec3& colorTrans);

	// Activar/desactivar la textura
	/* Activa la textura y mezcla la luz de la escena con el mix dado
	->REPLACE: no se tienen en cuenta las luces
	->MODULATE: luz * textura
	->ADD: luz + textura (efecto quemado) */
	void bind(GLuint mix = GL_MODULATE);

	/* Desactiva cualquier textura que estuviera activa */
	static void unbind(GLenum type = GL_TEXTURE_2D);

	/* Salva el contenido del color buffer dado en un archivo BMP */
	static void save(const std::string& BMP_Name, GLenum buf = GL_BACK);

	//Getters
	/* Ancho de la textura, en p�xeles */
	inline GLuint getWidth() const{ return w; };

	/* Alto de la textura, en p�xeles */
	inline GLuint getHeight() const { return h; };

	// Otros
	/* Activa o desactiva el uso de mipmaps */
	void useMipmaps(bool b);

	//
	inline bool hasAlpha() const { return m_hasAlpha; }

#ifdef __DEBUG_INFO__
	static GLuint numBinds;
#endif

protected:
	/* Textura activa actualmente */
	static GLuint s_activeTexture;

	// Tipo de textura que es
	GLenum texType;

	// Dimensiones de la textura (en p�xeles)
	GLuint w;
	GLuint h;

	/* Identificador interno (GPU) de la textura */
	GLuint id;

	/* Indica si la textura puede usar mipmaps o no */
	bool hasMipmap;

	/* Indica si la textura contiene transparencias */
	bool m_hasAlpha = false;

	// M�todos privados
	/* Inicializa la textura si no se hab�a creado ya */
	void Init();
};

// - - - - - - - - - - - - -

// Clase espec�fica para cubemaps. TODO: unificar
class CubemapTexture : public Texture
{
public:
	bool load(std::vector<std::string> faces);
};

#endif
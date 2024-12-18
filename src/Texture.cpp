#include "Texture.h"

#include "Pixmap32RGBA.h"
#include <iostream>

void Texture::Init()
{
	// Genera una nueva textura y devuelve un identificador para acceder a ella
	glGenTextures(1, &id); 
	// Indica a OpenGL que todo lo que hagamos con texturas será con la especificada
	glBindTexture(GL_TEXTURE_2D, id); 

	// Tipos de filtros de magnificación y minificación
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Tipos de "texture wrapping" en ambos ejes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

bool Texture::load(const std::string& filePath, GLubyte alpha)
{
	// Crear la textura y establecer los filtros
	if (id == 0) 
		Init();

	// Cargamos la información del BMP y le añadimos la 
	PixMap32RGBA pixMap; 
	pixMap.load_bmp24BGR(filePath); 
	if (pixMap.is_null())
		return false;

	if (alpha != 255) // transparencia especificada
		pixMap.set_alpha(alpha);

	// Dimensiones
	w = pixMap.width();
	h = pixMap.height();

	// Rellena la ya creada textura con el array de colores del BMP
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixMap.data());

	return true;
}

void Texture::bind(GLuint mix)
{
	glBindTexture(GL_TEXTURE_2D, id);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mix);
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::loadColorBuffer(GLsizei width, GLsizei height, GLenum buf)
{
	glReadBuffer(buf);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);
}

void Texture::save(const std::string& BMP_Name)
{
	// Crea el mapa de píxeles con las dimensiones de la pantalla
	PixMap32RGBA pixMap;
	pixMap.create_pixmap(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	// Lo rellena con la info obtenida del color buffer y lo guarda en formato BMP
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixMap.data());
	pixMap.save_bmp24BGR("..\\bin\\assets\\" + BMP_Name);
}
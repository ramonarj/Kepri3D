#include "Texture.h"

#include "Pixmap32RGBA.h"
#include <iostream>
#include <freeglut.h>

void Texture::Init()
{
	// Genera una nueva textura y devuelve un identificador para acceder a ella
	glGenTextures(1, &id); 
	// Indica a OpenGL que todo lo que hagamos con texturas será con la especificada
	glBindTexture(GL_TEXTURE_2D, id); 

	// Tipos de filtros de magnificación y minificación.
	// Los 'mipmaps' solo se usan para minificación, no magnificación
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

	// Genera un mipmap para la textura
	glGenerateMipmap(GL_TEXTURE_2D);

	return true;
}

bool Texture::loadRTT(GLsizei width, GLsizei height, GLenum buf)
{
	// 1) Leer la información del color buffer
	glReadBuffer(buf);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);

	// Crea el mapa de píxeles con las dimensiones de la pantalla
	PixMap32RGBA pixMap;
	pixMap.create_pixmap(width, height); // width, height

	// Lo rellena con la info obtenida del color buffer
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixMap.data());

	// 2) Crear la textura y establecer los filtros
	if (id == 0)
		Init();

	// Dimensiones
	this->w = width;
	this->h = height;

	// Rellena la ya creada textura con el array de colores del BMP
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixMap.data());

	// No generamos mipmap

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

void Texture::useMipmaps(bool b)
{
	glBindTexture(GL_TEXTURE_2D, id);

	// Se activa/desactiva el filtro de minificación que cambia entre texturas de distintos tamaños (mipmaps)
	if(b)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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
	try
	{
		pixMap.save_bmp24BGR("..\\bin\\assets\\textures\\" + BMP_Name);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}
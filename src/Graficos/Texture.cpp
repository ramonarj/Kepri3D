#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Pixmap32RGBA.h"
#include <iostream>
#include <freeglut.h>

const GLint COLOR_SPACE = GL_SRGB_ALPHA;

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
	hasMipmap = true;

	return true;
}

bool Texture::load(const std::string& filePath, const glm::ivec3& colorTrans)
{
	if (id == 0)
		Init();

	PixMap32RGBA::rgba_color colorPix = { colorTrans.r, colorTrans.g, colorTrans.b, 0 };
	PixMap32RGBA pixMap; // var. local para cargar la imagen del archivo
	pixMap.load_bmp24BGR(filePath); // carga y añade alpha=255
	// carga correcta?
	if (!pixMap.is_null())
	{
		//glAlphaFunc(GL_GREATER, 0.0); // no haría falta si lo establece Game
		pixMap.set_colorkey_alpha(colorPix, 0);

		w = pixMap.width();
		h = pixMap.height();

		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, COLOR_SPACE, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixMap.data());

		// No es recomendable usar mipmap con este modelo (se pierde la transparencia al alejarnos)
		hasMipmap = false;
		//glGenerateMipmap(GL_TEXTURE_2D);

		return true;
	}
	else
		return false;
}

bool Texture::loadSTBI(const std::string& filePath)
{
	// Crear la textura y establecer los filtros
	if (id == 0)
		Init();

	// Cargamos la información del archivo de imagen (BMP, JPG, PNG, etc).
	// Siempre usamos 4 canales, aunque no sea un PNG
	int numChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filePath.c_str(), (int*) & w, (int*) & h, &numChannels, 4);

	// Rellena la ya creada textura con el array de píxeles
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, COLOR_SPACE, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	// Genera un mipmap
	glGenerateMipmap(GL_TEXTURE_2D);
	hasMipmap = true;

	// Liberar la memoria y volver
	stbi_image_free(data);
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
	if(b && hasMipmap)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Texture::loadColorBuffer(GLsizei width, GLsizei height, GLenum buf)
{
	glReadBuffer(buf);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, COLOR_SPACE, 0, 0, width, height, 0);
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


// - - - - - - - - - - - - - - - - - - -


bool CubemapTexture::load(std::vector<std::string> faces)
{
	// Crear la textura de tipo CubeMap
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	// Cargar las 6 texturas de los lados
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		// Cargamos la información del archivo de imagen
		int numChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(faces[i].c_str(), (int*)&w, (int*)&h, &numChannels, 4);

		// Rellena la ya creada textura con el array de píxeles
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, COLOR_SPACE,
			w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		// Parámetros de escalado y repetición
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// IMPORTANTE: la tercera dimensión
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// Liberar la memoria
		stbi_image_free(data);
	}

	// No generamos mipmap
	hasMipmap = false;

	return true;
}

void CubemapTexture::bind(GLuint mix)
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mix); //REPLACE sería lo correcto
}

void CubemapTexture::unbind()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
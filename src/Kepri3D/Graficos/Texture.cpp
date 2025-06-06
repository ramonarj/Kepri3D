#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "ResourceManager.h"
#include "Pixmap32RGBA.h"
#include <iostream>
#include <freeglut.h>

#ifdef __DEBUG_INFO__
GLuint Texture::numBinds = 0;
#endif

GLuint Texture::s_activeTexture = 0;

Texture::Texture(GLenum type) : w(0), h(0), id(0), hasMipmap(false), m_hasAlpha(false)
{
	Init(type);
}

Texture::~Texture()
{
	if (id != 0) 
		glDeleteTextures(1, &id);
}

void Texture::Init(GLenum type)
{
	// Tipo de textura que es (2D, multisample, cubemap...)
	texType = type;

	// Genera una nueva textura y devuelve un identificador para acceder a ella
	glGenTextures(1, &id); 

	// Indica a OpenGL que todo lo que hagamos con texturas ser� con la especificada
	glBindTexture(texType, id);
}

bool Texture::load(const std::string& filePath, GLubyte alpha, GLint intFormat)
{
	// Crear la textura y establecer los filtros
	if (id == 0)
		Init();
	setFilters(GL_LINEAR, GL_REPEAT);

	// Cargamos la informaci�n del archivo de imagen (BMP, JPG, PNG, etc).
	// Siempre usamos 4 canales, aunque no sea un PNG
	int numChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filePath.c_str(), (int*)&w, (int*)&h, &numChannels, 4);
	if (data == nullptr) { return false; }

	// Dar la transparencia indicada
	if(alpha != 255)
	{
		m_hasAlpha = true;
		float alphaFl = alpha / 255.0f;
		for (int i = 0; i < (w * h); i++)
			data[i * 4 + 3] *= alphaFl;
	}
	// Comprobar si tiene transparencias. TODO: tendr�a que haber una forma mejor que recorrer los p�xeles de nuevo
	if(!m_hasAlpha)
	{
		int i = 0;
		while(i < (w * h) && !m_hasAlpha)
		{
			if (data[i * 4 + 3] < 255)
				m_hasAlpha = true;
			i++;
		}
	}

	// Rellena la ya creada textura con el array de p�xeles
	glBindTexture(texType, id);
	glTexImage2D(texType, 0, intFormat, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	// Genera un mipmap
	glGenerateMipmap(texType);
	hasMipmap = true;

	// Liberar la memoria y volver
	stbi_image_free(data);
	return true;
}

bool Texture::load(const std::string& filePath, const glm::ivec3& colorTrans)
{
	if (id == 0)
		Init();
	setFilters(GL_LINEAR, GL_REPEAT);

	PixMap32RGBA::rgba_color colorPix = { colorTrans.r, colorTrans.g, colorTrans.b, 0 };
	PixMap32RGBA pixMap; // var. local para cargar la imagen del archivo
	pixMap.load_bmp24BGR(filePath); // carga y a�ade alpha=255
	// carga correcta?
	if (!pixMap.is_null())
	{
		//glAlphaFunc(GL_GREATER, 0.0); // no har�a falta si lo establece Game
		pixMap.set_colorkey_alpha(colorPix, 0);

		w = pixMap.width();
		h = pixMap.height();

		glBindTexture(texType, id);
		glTexImage2D(texType, 0, COLOR_SPACE, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixMap.data());

		// No es recomendable usar mipmap con este modelo (se pierde la transparencia al alejarnos)
		hasMipmap = false;
		//glGenerateMipmap(GL_TEXTURE_2D);

		return true;
	}
	else
		return false;
}



void Texture::bind(GLuint mix)
{
	// Esto solo ser� �til en caso de que las entidades se pinten ordenadas por su textura
	if (s_activeTexture == id) { return; }

	glBindTexture(texType, id);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mix);
	s_activeTexture = id;
#ifdef __DEBUG_INFO__
	numBinds++;
#endif
}

void Texture::unbind(GLenum type)
{
	if (s_activeTexture == 0) { return; }

	glBindTexture(type, 0);
	s_activeTexture = 0;
#ifdef __DEBUG_INFO__
	numBinds++;
#endif
}

void Texture::useMipmaps(bool b)
{
	bind();

	// Se activa/desactiva el filtro de minificaci�n que cambia entre texturas de distintos tama�os (mipmaps)
	if(b && hasMipmap)
		glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	else
		glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Texture::save(const std::string& BMP_Name, GLenum buf)
{
	GLuint w = glutGet(GLUT_WINDOW_WIDTH);
	GLuint h = glutGet(GLUT_WINDOW_HEIGHT);

	// Lee los p�xeles del Color Buffer
	glReadBuffer(buf);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, COLOR_SPACE, 0, 0, w, h, 0);

	// Crea el mapa de p�xeles con las dimensiones de la pantalla
	PixMap32RGBA pixMap;
	pixMap.create_pixmap(w, h);

	// Lo rellena con la info obtenida del color buffer y lo guarda en formato BMP
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixMap.data());
	try
	{
		pixMap.save_bmp24BGR(ResourceManager::ASSETS_PATH + BMP_Name);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

void Texture::setFilters(GLenum minMagFilter, GLenum wrapFilter)
{
	// Tipos de filtros de magnificaci�n y minificaci�n.
	// Los 'mipmaps' solo se usan para minificaci�n, no magnificaci�n
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, minMagFilter);
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, minMagFilter);

	// Tipos de "texture wrapping" en ambos ejes
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, wrapFilter);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, wrapFilter);
}

// - - - - - - - - - - - - -

Texture* Texture::createColorAttachment(unsigned int w, unsigned int h, unsigned int samples)
{
	GLenum texType = (samples > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

	// Crear la textura
	Texture* t = new Texture(texType);

	// �Multisampled?
	if(texType == GL_TEXTURE_2D_MULTISAMPLE)
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, COLOR_SPACE, w, h, GL_TRUE);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, COLOR_SPACE, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	// Filtros
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// La atamos
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texType, t->id, 0);
	return t;
}

Texture* Texture::createDepthAttachment(unsigned int w, unsigned int h, GLenum texType)
{
	// Crear la textura
	Texture* t = new Texture(texType);
	// Textura normal
	if (texType == GL_TEXTURE_2D)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		t->w = w; t->h = h;
		// Filtros
		t->setFilters(GL_NEAREST, GL_CLAMP_TO_BORDER);

		// Color del borde
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		// La atamos
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, t->id, 0);
	}
	// Cubemap
	else if (texType == GL_TEXTURE_CUBE_MAP)
	{
		// Crear las 6 caras del cubemap
		for (unsigned int i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
				w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		t->w = w; t->h = h;
		// Filtros
		t->setFilters(GL_NEAREST, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// La atamos
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, t->id, 0);
	}
	else
		return nullptr;

	return t;
}

// - - - - - - - - - - - - - - - - - - -


bool CubemapTexture::load(std::vector<std::string> faces)
{
	// Crear la textura de tipo CubeMap
	Init(GL_TEXTURE_CUBE_MAP);

	// Cargar las 6 texturas de los lados
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		// Cargamos la informaci�n del archivo de imagen
		int numChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(faces[i].c_str(), (int*)&w, (int*)&h, &numChannels, 4);

		// Rellena la ya creada textura con el array de p�xeles
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, COLOR_SPACE,
			w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		// Par�metros de escalado y repetici�n
		setFilters(GL_LINEAR, GL_CLAMP_TO_EDGE);
		// IMPORTANTE: la tercera dimensi�n
		glTexParameteri(texType, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// Liberar la memoria
		stbi_image_free(data);
	}

	// Generamos mipmap
	glGenerateMipmap(texType);
	hasMipmap = true;

	return true;
}
#ifndef __BUFFER_OBJECTS__
#define __BUFFER_OBJECTS__

#include <glew.h>
#include <glm.hpp>

class Framebuffer
{
public:
	Framebuffer(){}
	Framebuffer(GLuint width, GLuint height, bool multisampling = true);
	~Framebuffer();

	/* Establece este framebuffer para pintar sobre él */
	inline void bind(GLenum target = GL_FRAMEBUFFER) { glBindFramebuffer(target, id); }

	/* Desactiva el framebuffer que hubiera, volviendo a dejar el predeterminado de OpenGL */
	inline static void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

	/* Desactiva el framebuffer, volviendo a dejar el predeterminado */
	void bindTexture(GLenum target = GL_TEXTURE_2D) { glBindTexture(target, textureId); }

	/* Crea un mapa de sombras con la resolución dada */
	static Framebuffer* createShadowMap(unsigned int width, unsigned int height, bool omnidirectional = false);

private:
	unsigned int id;
	unsigned int textureId;
	unsigned int renderbufId;
};

// - - - - - - - - - - - 

class Uniformbuffer
{
public:
	Uniformbuffer(GLuint index, GLsizeiptr bufSize);
	~Uniformbuffer(){}

	/* Activa este Uniform Buffer */
	inline void bind() { glBindBuffer(GL_UNIFORM_BUFFER, id); }

	/* Desactiva el Uniform Buffer  que hubiera activo*/
	inline static void unbind() { glBindBuffer(GL_UNIFORM_BUFFER, 0); }
private:
	unsigned int id;
};

// - - - - - - - - - - - 

class Vertexbuffer
{
public:
	Vertexbuffer(void* vertices, unsigned int numVertices, unsigned int indice = 0);
	~Vertexbuffer() {}

	/* Activa este VBO */
	inline void bind() { glBindBuffer(GL_ARRAY_BUFFER, id); }

	/* Desactiva el VBO  que hubiera activo*/
	inline static void unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

	void updateData(void* vertices);
private:
	unsigned int id;
	unsigned int bufSize;
};

// - - - - - - - - - - - 

class Elementbuffer
{
public:
	Elementbuffer(void* indices, unsigned int numIndices);
	~Elementbuffer() {}

	/* Activa este buffer */
	inline void bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id); }

	/* Desactiva el buffer que hubiera activo*/
	inline static void unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

private:
	unsigned int id;
};

// - - - - - - - - - - - 

// Técnicamente no es un buffer object, sino un contenedor de estos
class VertexArray
{
public:
	VertexArray();
	~VertexArray() {}

	/* Activa este VAO */
	inline void bind() { glBindVertexArray(id); }

	/* Desactiva el VAO que hubiera actibo*/
	inline static void unbind() { glBindVertexArray(0); }

private:
	unsigned int id;
};

#endif
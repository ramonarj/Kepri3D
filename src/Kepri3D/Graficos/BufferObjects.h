#ifndef __BUFFER_OBJECTS__
#define __BUFFER_OBJECTS__

#include <glew.h>

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
	void bindTexture() { glBindTexture(GL_TEXTURE_2D, textureId); }

	/* Crea un mapa de sombras con la resolución dada */
	static Framebuffer* createShadowMap(unsigned int width, unsigned int height);

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
	inline void bind(GLenum target = GL_FRAMEBUFFER) { glBindBuffer(GL_UNIFORM_BUFFER, id); }

	/* Desactiva el Uniform Buffer  que hubiera activo*/
	inline static void unbind() { glBindBuffer(GL_UNIFORM_BUFFER, 0); }
private:
	unsigned int id;
};

#endif
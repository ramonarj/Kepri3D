#ifndef __FRAMEBUFFER__
#define __FRAMEBUFFER__

#include <glew.h>

class Framebuffer
{
public:
	Framebuffer(GLuint width, GLuint height);
	~Framebuffer();

	/* Establece este framebuffer para pintar sobre él */
	void bind() { glBindFramebuffer(GL_FRAMEBUFFER, id); }

	/* Desactiva el framebuffer que hubiera, volviendo a dejar el predeterminado de OpenGL */
	static void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

	/* Desactiva el framebuffer, volviendo a dejar el predeterminado */
	void bindTexture() { glBindTexture(GL_TEXTURE_2D, textureId); }
private:
	unsigned int id;
	unsigned int textureId;
	unsigned int renderbufId;
};

#endif
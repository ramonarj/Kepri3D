#include "BufferObjects.h"

#include "Texture.h"
#include <freeglut.h>
#include <iostream>

Framebuffer::Framebuffer(GLuint width, GLuint height, bool multisampling)
{
    // Creamos un Framebuffer. Debe tener 3 elementos atados a él: Color, Depth y Stencil buffer
    glGenFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    // Con multisampling
    if(multisampling)
    {
        // Creamos una textura que servirá de Color Buffer
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureId);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, COLOR_SPACE, width, height, GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureId, 0); // la atamos

        // Creamos un Renderbuffer que servirá de Depth+Stencil buffer
        glGenRenderbuffers(1, &renderbufId);
        glBindRenderbuffer(GL_RENDERBUFFER, renderbufId);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLES, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbufId); // lo atamos
    }
    // sin
    else
    {
        // Lo mismo pero sin multisamples
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, COLOR_SPACE, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);

        glGenRenderbuffers(1, &renderbufId);
        glBindRenderbuffer(GL_RENDERBUFFER, renderbufId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbufId);
    }

    // Comprobamos que todo ha ido bien
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer* Framebuffer::createShadowMap(unsigned int width, unsigned int height)
{
    Framebuffer* fb = new Framebuffer();
    glGenFramebuffers(1, &fb->id);

    // create depth texture
    glGenTextures(1, &fb->textureId);
    glBindTexture(GL_TEXTURE_2D, fb->textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, fb->id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fb->textureId, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return fb;
}

Framebuffer::~Framebuffer()
{

}

// - - - - - - - - - - - - - - - 

Uniformbuffer::Uniformbuffer(GLuint index, GLsizeiptr bufSize)
{
    // Crea el UBO
    glGenBuffers(1, &id);
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    // Reserva el espacio necesario para el buffer
    glBufferData(GL_UNIFORM_BUFFER, bufSize, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // Unir el UBO creado al punto de unión dado por 'index'
    glBindBufferRange(GL_UNIFORM_BUFFER, index, id, 0, bufSize);
}
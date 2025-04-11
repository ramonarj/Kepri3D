#include "BufferObjects.h"

#include "Game.h"
#include "Texture.h"
#include <freeglut.h>
#include <iostream>

#include <gtc/type_ptr.hpp>

Framebuffer::Framebuffer(GLuint width, GLuint height, bool multisampling) : depthTexture(nullptr)
{
    // Creamos un Framebuffer. Debe tener 3 elementos atados a él: Color, Depth y Stencil buffer
    glGenFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    // Con multisampling
    if(multisampling)
    {
        // Creamos una textura tipo MULTISAMPLE que servirá de Color Buffer
        texture = Texture::createColorAttachment(width, height, MSAA_SAMPLES);
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureId, 0);

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
        texture = Texture::createColorAttachment(width, height);

        glGenRenderbuffers(1, &renderbufId);
        glBindRenderbuffer(GL_RENDERBUFFER, renderbufId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbufId);
    }

    // Comprobamos que todo ha ido bien
    checkComplete();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer* Framebuffer::createShadowMap(unsigned int width, unsigned int height, bool omnidirectional)
{
    Framebuffer* fb = new Framebuffer();
    glGenFramebuffers(1, &fb->id);
    glBindFramebuffer(GL_FRAMEBUFFER, fb->id);

    // Luces puntuales
    if(omnidirectional)
        fb->texture = Texture::createDepthAttachment(width, height, GL_TEXTURE_CUBE_MAP);
    // Luces direccionales
    else
        fb->texture = Texture::createDepthAttachment(width, height, GL_TEXTURE_2D);

    // No queremos usar el Color Buffer para nada
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Comprobamos que todo ha ido bien
    checkComplete();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return fb;
}

Framebuffer* Framebuffer::createMRTBuffer(unsigned int width, unsigned int height)
{
    Framebuffer* fb = new Framebuffer();
    glGenFramebuffers(1, &fb->id);
    glBindFramebuffer(GL_FRAMEBUFFER, fb->id);

    // - Color Buffer - //
    fb->texture = Texture::createColorAttachment(width, height);

    // - - Depth Buffer - - //
    fb->depthTexture = Texture::createDepthAttachment(width, height, GL_TEXTURE_2D);

    // Comprobamos que todo ha ido bien
    checkComplete();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return fb;
}

void Framebuffer::checkComplete()
{
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
}

void Framebuffer::bindTexture() 
{
    if (texture != nullptr) { texture->bind(); }
}

void Framebuffer::bindDepth()
{
    if (depthTexture != nullptr) { depthTexture->bind(); }
}

Framebuffer::~Framebuffer()
{
    if(texture != nullptr) delete texture;
    if (depthTexture != nullptr) delete depthTexture;
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

// - - - - - - - - - - - - - - - 

Vertexbuffer::Vertexbuffer(void* vertices, unsigned int numVertices, unsigned int indice)
{
    // Crea el VBO
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);

    // Le da un espacio concreto y lo rellena con la información de los vértices
    bufSize = sizeof(double) * 3 * numVertices;
    glBufferData(GL_ARRAY_BUFFER, bufSize, vertices, GL_STATIC_DRAW);
    // Tipos de buffer: GL_STREAM_DRAW | GL_STATIC_DRAW | GL_DYNAMIC_DRAW
    
    // Indica el formato de la información dada y activa el atributo 0 del VS, que es donde irá
    glVertexAttribPointer(indice, 3, GL_DOUBLE, GL_FALSE, 0, (void*)0); // stride-> 3 * sizeof(double)
    glEnableVertexAttribArray(indice);

    //glBindBuffer(GL_ARRAY_BUFFER, 0);   
}

void Vertexbuffer::updateData(void* vertices)
{
    glBufferSubData(GL_ARRAY_BUFFER, 0, bufSize, vertices);
}

// - - - - - - - - - - - - - - - 

Elementbuffer::Elementbuffer(void* indices, unsigned int numIndices)
{
    // Crea el EBO
    glGenBuffers(1, &id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);

    // Le da un espacio concreto y lo rellena con la información de los índices
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numIndices, indices, GL_STATIC_DRAW);

    // Importante NO llamar al unbind del EBO cuando estemos creando un VAO hasta hacer unbind del VAO mismo
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// - - - - - - - - - - - - - - - 

VertexArray::VertexArray()
{
    // Crea el VAO (y ya)
    glGenVertexArrays(1, &id);
    glBindVertexArray(id);

    //

    glBindVertexArray(0);
}
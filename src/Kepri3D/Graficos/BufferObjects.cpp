#include "BufferObjects.h"

#include "Texture.h"
#include <freeglut.h>
#include <iostream>

#include <gtc/type_ptr.hpp>

Framebuffer::Framebuffer(GLuint width, GLuint height, bool multisampling)
{
    // Creamos un Framebuffer. Debe tener 3 elementos atados a �l: Color, Depth y Stencil buffer
    glGenFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    // Con multisampling
    if(multisampling)
    {
        // Creamos una textura que servir� de Color Buffer
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureId);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, COLOR_SPACE, width, height, GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureId, 0); // la atamos

        // Creamos un Renderbuffer que servir� de Depth+Stencil buffer
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

Framebuffer* Framebuffer::createShadowMap(unsigned int width, unsigned int height, bool omnidirectional)
{
    Framebuffer* fb = new Framebuffer();
    glGenFramebuffers(1, &fb->id);

    // Para luces direccionales
    if(!omnidirectional)
    {
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
    }
    // Para luces puntuales
    else
    {
        glGenTextures(1, &fb->textureId);
        glBindTexture(GL_TEXTURE_CUBE_MAP, fb->textureId);
        // Crear las 6 caras del cubemap
        for (unsigned int i = 0; i < 6; ++i)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, 
                width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, fb->id);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, fb->textureId, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    // Comprobamos que todo ha ido bien
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

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
    // Unir el UBO creado al punto de uni�n dado por 'index'
    glBindBufferRange(GL_UNIFORM_BUFFER, index, id, 0, bufSize);
}

// - - - - - - - - - - - - - - - 

Vertexbuffer::Vertexbuffer(void* vertices, unsigned int numVertices, unsigned int indice)
{
    // Crea el VBO
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);

    // Le da un espacio concreto y lo rellena con la informaci�n de los v�rtices
    bufSize = sizeof(double) * 3 * numVertices;
    glBufferData(GL_ARRAY_BUFFER, bufSize, vertices, GL_STATIC_DRAW);
    // Tipos de buffer: GL_STREAM_DRAW | GL_STATIC_DRAW | GL_DYNAMIC_DRAW
    
    // Indica el formato de la informaci�n dada y activa el atributo 0 del VS, que es donde ir�
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

    // Le da un espacio concreto y lo rellena con la informaci�n de los �ndices
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
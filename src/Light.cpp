#include "Light.h"

#include <gtc/type_ptr.hpp>

GLuint Light::cont = 0;

Light::Light(glm::fvec4 diffuse) : id(GL_MAX_LIGHTS), posDir({0,0,0,0})
{
	if(cont < GL_MAX_LIGHTS)
	{
		// El id tiene que empezar a partir de LIGHT0 (0x4000)
		id = GL_LIGHT0 + cont;
		glEnable(id); // es como hacer glEnable(GL_LIGHTid)
		cont++;

		ambient = { 0.1,0.1,0.1,1 };
		//ambient = { 0.5,0.5,0.5,1 };
		this->diffuse = diffuse;
		specular = { 0.5,0.5,0.5,1 };
		//specular = { 0,0,0,1 };
	}
}

Light::~Light()
{
	setActive(false);
}

void Light::load(glm::dmat4 viewMat)
{
	// La luz tiene que moverse a la inversa de la cámara igual que las entidades
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(value_ptr(viewMat));

	// Posición /dirección de la luz
	glLightfv(id, GL_POSITION, value_ptr(posDir));

	//fv = float vector (puntero a)
	// Valores de la luz
	glLightfv(id, GL_AMBIENT, value_ptr(ambient));
	glLightfv(id, GL_DIFFUSE, value_ptr(diffuse));
	glLightfv(id, GL_SPECULAR, value_ptr(specular));
}

void Light::setActive(bool active)
{
	if(id < GL_LIGHT0 + GL_MAX_LIGHTS)
	{
		if(active)
			glEnable(id);
		else
			glDisable(id);
	}
}
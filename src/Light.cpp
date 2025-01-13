#include "Light.h"

#include <gtc/type_ptr.hpp>

GLuint Light::cont = 0;

Light::Light(LightType type, glm::fvec4 diffuse) : id(GL_MAX_LIGHTS), m_active(true), posDir({0,0,0,0})
{
	this->type = type;

	// Valores por defecto para la atenuación de la luz
	this->constantAtt = 1;
	this->linearAtt = 0;
	this->quadraticAtt = 0;

	// Valor por defecto para focos (hacia abajo)
	this->spotDir = { 0,-1,0 };
	this->spotAngle = 180.0f;
	this->spotExp = 0.0f;

	// Asegurarnos de que cabe otra luz más
	if(cont < GL_MAX_LIGHTS)
	{
		// El id tiene que empezar a partir de LIGHT0 (0x4000)
		id = GL_LIGHT0 + cont;
		glEnable(id); // es como hacer glEnable(GL_LIGHTid)
		cont++;

		this->ambient = { 0.2,0.2,0.2,1 };
		this->diffuse = diffuse;
		this->specular = { 1,1,1,1 };
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

	// Radio y ángulo (SOLO para focos)
	if (type == SPOT_LIGHT)
	{
		// Valores por defecto: 180º, 0 y (0,0,-1)
		glLightf(id, GL_SPOT_CUTOFF, spotAngle);
		glLightf(id, GL_SPOT_EXPONENT, spotExp); // entre 0 - 128
		glLightfv(id, GL_SPOT_DIRECTION, value_ptr(spotDir));
	}

	// Atenuación de la luz con la distancia (para luces no direccionales)
	if(type != DIRECTIONAL_LIGHT)
	{
		glLightf(id, GL_CONSTANT_ATTENUATION, constantAtt);
		glLightf(id, GL_LINEAR_ATTENUATION, linearAtt);
		glLightf(id, GL_QUADRATIC_ATTENUATION, quadraticAtt);
	}
}

void Light::setActive(bool active)
{
	m_active = active;
	if(id < GL_LIGHT0 + GL_MAX_LIGHTS)
	{
		if(active)
			glEnable(id);
		else
			glDisable(id);
	}
}

void Light::setAttenuationFactors(float constant, float linear, float quadratic)
{
	this->constantAtt = constant;
	this->linearAtt = linear;
	this->quadraticAtt = quadratic;
}

void Light::setSpotlightDir(glm::fvec3 dir)
{
	spotDir = dir;
}

void Light::setSpotlightAngle(GLfloat angle)
{
	spotAngle = angle / 2.0f;
}

void Light::setSpotlightHardness(GLfloat hardness)
{
	spotExp = 128.0 - hardness;
}
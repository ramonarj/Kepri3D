#include "Material.h"

#include <freeglut.h>
#include <gtc/type_ptr.hpp>

Material::Material()
{
	// Valores por defecto
	m_ambient = { 0.2, 0.2, 0.2, 1.0 };
	m_diffuse = { 0.8, 0.8, 0.8, 1.0 };
	m_specular = { 0.0, 0.0, 0.0, 1.0 };
	m_expF = 0;
}

Material::Material(glm::fvec4 ambient, glm::fvec4 diffuse, glm::fvec4 specular, GLfloat expF)
{
	m_ambient = ambient;;
	m_diffuse = diffuse;
	m_specular = specular;

	m_expF = expF;
}

void Material::load()
{
	//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, value_ptr(m_ambient));
	//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, value_ptr(m_diffuse));

	// El "front and back" produce algunos efectos no deseados
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, value_ptr(m_diffuse));
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, value_ptr(m_specular));

	// Esto hay que hacerlo aunque ya se establezca al inicio del programa
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_expF);
	glShadeModel(GL_SMOOTH);
}
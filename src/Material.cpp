#include "Material.h"

#include <freeglut.h>
#include <gtc/type_ptr.hpp>

Material::Material()
{
	// Valores por defecto
	m_ambient = { 0.2, 0.2, 0.2, 1.0 };
	m_diffuse = { 0.8, 0.8, 0.8, 1.0 };
	m_specular = { 0.0, 0.0, 0.0, 1.0 };
	m_brillo = 0;
	m_face = GL_FRONT_AND_BACK;
}

Material::Material(glm::fvec4 ambient, glm::fvec4 diffuse, glm::fvec4 specular, GLfloat brillo)
{
	m_ambient = ambient;;
	m_diffuse = diffuse;
	m_specular = specular;

	m_brillo = brillo;
	m_face = GL_FRONT_AND_BACK;
}

void Material::load()
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, value_ptr(m_diffuse));
	// El "front and back" produce algunos efectos no deseados
	//glMaterialfv(m_face, GL_AMBIENT, value_ptr(m_ambient));
	//glMaterialfv(m_face, GL_DIFFUSE, value_ptr(m_diffuse));
	glMaterialfv(m_face, GL_SPECULAR, value_ptr(m_specular));

	glMaterialf(m_face, GL_SHININESS, m_brillo);

	// Esto hay que hacerlo aunque ya se establezca al inicio del programa
	glShadeModel(GL_SMOOTH);
}
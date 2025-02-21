#include "Material.h"
#include <gtc/type_ptr.hpp>

#include "Texture.h"
#include "Shader.h"
#include <string>

GLuint Material::m_shading;

Material::Material()
{
	// Valores por defecto
	m_ambient = { 0.2, 0.2, 0.2, 1.0 };
	m_diffuse = { 0.8, 0.8, 0.8, 1.0 };
	m_specular = { 0.1, 0.1, 0.1, 1.0 };
	m_emission = { 0.0, 0.0, 0.0, 1.0 };
	m_brillo = 8;
	m_face = GL_FRONT;
	for (int i = 0; i < NUM_TEXTURES; i++)
		m_textures[i] = nullptr;
}

Material::Material(glm::fvec4 ambient, glm::fvec4 diffuse, glm::fvec4 specular, glm::fvec4 emission, GLfloat brillo)
{
	m_ambient = ambient;
	m_diffuse = diffuse;
	m_specular = specular;
	m_emission = emission;

	m_brillo = brillo;
	m_face = GL_FRONT;
	for (int i = 0; i < NUM_TEXTURES; i++)
		m_textures[i] = nullptr;
}

void Material::load()
{
	//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, value_ptr(m_diffuse));
	// El "front and back" produce algunos efectos no deseados
	glMaterialfv(m_face, GL_AMBIENT, value_ptr(m_ambient));
	glMaterialfv(m_face, GL_DIFFUSE, value_ptr(m_diffuse));
	glMaterialfv(m_face, GL_SPECULAR, value_ptr(m_specular));

	glMaterialf(m_face, GL_SHININESS, m_brillo);

	glMaterialfv(m_face, GL_EMISSION, value_ptr(m_emission));

	// Esto hay que hacerlo aunque ya se establezca al inicio del programa
	glShadeModel(m_shading);
}

void Material::loadToShader(Shader* sh)
{
	if (sh == nullptr)
		return;

	// Propiedades del material
	sh->setVec3("material.ambient", m_ambient);
	sh->setVec4("material.diffuse", m_diffuse);
	sh->setVec3("material.specular", m_specular);
	sh->setFloat("material.brillo", m_brillo);

	// Enviar las texturas necesarias al shader
	bindTextures(sh);
}

void Material::bindTextures(Shader* sh)
{
	// Nombre que deben tener las respectivas variables 'sampler2D' del shader
	std::string texNames[NUM_TEXTURES] = { "textura", "textura2", "material.specular_map", "normalMap",
		"dispMap", "reflectionMap", "skybox" };

	// Activar cada textura existente
	for (int i = 0; i < NUM_TEXTURES; i++)
	{
		if (m_textures[i] != nullptr)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			m_textures[i]->bind();
			sh->setInt(texNames[i], i);
		}
	}

	// Booleanos extra
	if (m_textures[0] != nullptr)
		sh->setInt("use_diff_map", true);
	else
		sh->setInt("use_diff_map", false);

	if (m_textures[2] != nullptr)
		sh->setInt("use_spec_map", true);
	else
		sh->setInt("use_spec_map", false);

	if (m_textures[3] != nullptr)
		sh->setInt("use_normal_map", true);
	else
		sh->setInt("use_normal_map", false);
}

void Material::unload()
{
	// El método Texture::unbind() debería ser estático, solo hay que llamarlo 1 vez
	if (m_textures[0] != nullptr)
	{
		m_textures[0]->unbind();
		glActiveTexture(GL_TEXTURE0);
	}
}

#include "Material.h"
#include <gtc/type_ptr.hpp>

#include "Texture.h"
#include "Shader.h"
#include <string>

GLuint Material::m_shading;
bool Material::fresnel = true;
bool Material::s_useTextures = true;

// Nombre que deben tener las respectivas variables 'sampler2D/samplerCube' del shader
std::string g_texNames[NUM_TEXTURES] = { "textura", "textura2", "material.specular_map", "normalMap",
	"dispMap", "reflectionMap", "skybox", "material.emission_map" };

#define KEPRI_TEXTURE_0 10

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
	m_shader = nullptr;
	m_needsTangents = false;
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
	m_shader = nullptr;
	m_needsTangents = false;
}

void Material::setTexture(unsigned int i, Texture* tex)
{
	m_textures[i] = tex;
	// Si es un normal map, necesitaremos tangentes de la malla
	if (i == 3 || i == 4)
		m_needsTangents = true;
}

void Material::load()
{
	// Bindear la textura principal
	glActiveTexture(GL_TEXTURE0);
	if (m_textures[0] != nullptr && s_useTextures)
		m_textures[0]->bind(); 
	else
		Texture::unbind(); 

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
	if (sh == nullptr) { return; }

	// Evitar enviar información de más a shaders que no la requieran
	// Pensado sobre todo para el Shadow Pass
	if(sh->useTextures())
	{
		// Globales
		sh->setInt("fresnel", fresnel);

		// Propiedades del material
		sh->setVec3("material.ambient", m_ambient);
		sh->setVec4("material.diffuse", m_diffuse);
		sh->setVec3("material.specular", m_specular);
		sh->setFloat("material.brillo", m_brillo);

		sh->setVec3("material.emission", m_emission);

		// Texturas
		if(s_useTextures)
			bindTextures(sh);
	}
	// Mandar los uniforms específicos de este material
	sendCustomUniforms(sh);
}

void Material::bindTextures(Shader* sh)
{
	int mask = 0;
	// Activar cada textura existente
	for (int i = 0; i < NUM_TEXTURES; i++)
	{
		if (m_textures[i] != nullptr)
		{
			// Activar la textura
			glActiveTexture(GL_TEXTURE0 + i);
			m_textures[i]->bind();
			sh->setInt(g_texNames[i], i);
			// Poner a 1 el bit de la máscara
			mask = mask | (1 << i);
		}
	}
	sh->setInt("material.mapsMask", mask);
}

void Material::sendCustomUniforms(Shader* sh)
{
	// Floats
	for (auto it : m_floatUniforms)
		sh->setFloat(it.first, it.second);
	// Enteros
	for (auto it : m_intUniforms)
		sh->setInt(it.first, it.second);
	// Vector2
	for (auto it : m_vec2Uniforms)
		sh->setVec2(it.first, it.second);
	// Vector3
	for (auto it : m_vec3Uniforms)
		sh->setVec3(it.first, it.second);
	// Texturas (de la 10 a la 19 están libres)
	int i = 0;
	for (auto it : m_texUniforms)
	{
		sh->setTexture(it.first, KEPRI_TEXTURE_0 + i, it.second);
		i++;
	}
}

void Material::setFloat(const std::string& name, float value)
{
	// Se guarda el valor en el material (en el Update()) para luego pasárselo al shader (en el render())
	m_floatUniforms[name] = value;
}

void Material::setInt(const std::string& name, int value)
{
	m_intUniforms[name] = value;
}

void Material::setVec2(const std::string& name, glm::vec2 value)
{
	m_vec2Uniforms[name] = value;
}

void Material::setVec3(const std::string& name, glm::vec3 value)
{
	m_vec3Uniforms[name] = value;
}

void Material::setTexture(const std::string& name, Texture* value)
{
	m_texUniforms[name] = value;
}
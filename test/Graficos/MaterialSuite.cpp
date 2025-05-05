#include "MaterialSuite.h"

#include "Graficos/Material.h"
#include "Graficos/Texture.h"
#include "Utils.h"

void MaterialSuite::setup()
{
	addTests({ getAmbient, getDiffuse, getSpecular, getEmission, getBrillo,
		getTexture, setTexture });
}


void MaterialSuite::getAmbient()
{
	Material m;
	glm::vec3 ambient = m.getAmbient();
	assert(sameVector(ambient, Material::DEFAULT_AMBIENT));
}

void MaterialSuite::getDiffuse()
{
	Material m;
	glm::vec3 diffuse = m.getDiffuse();
	assert(sameVector(diffuse, Material::DEFAULT_DIFFUSE));
}

void MaterialSuite::getSpecular()
{
	Material m;
	glm::vec3 specular = m.getSpecular();
	assert(sameVector(specular, Material::DEFAULT_SPECULAR));
}

void MaterialSuite::getEmission()
{
	Material m;
	glm::vec3 emission = m.getEmission();
	assert(sameVector(emission, Material::DEFAULT_EMISSION));
}

void MaterialSuite::getBrillo()
{
	Material m;
	float brillo = m.getBrillo();
	assert(brillo == Material::DEFAULT_BRILLO);
}

void MaterialSuite::getTexture()
{
	Material m;
	// Sin texturas
	const Texture* pTex = m.getTexture(0);
	assert(pTex == nullptr);

	pTex = m.getTexture(-1);
	assert(pTex == nullptr);

	pTex = m.getTexture(NUM_TEXTURES);
	assert(pTex == nullptr);

	// Con texturas
	Texture t, t2;
	m.setTexture(1, &t);
	assert(m.getTexture(0) == nullptr);
	assert(m.getTexture(1) == &t);
	m.setTexture(1, &t2);
	assert(m.getTexture(1) == &t2);
}

void MaterialSuite::setTexture()
{
	Material m;

	Texture t;
	// Casos límite
	m.setTexture(-1, &t);
	m.setTexture(NUM_TEXTURES, &t);

	// Need tangents (normal)
	m.setTexture(NORMAL_MAP, &t);
	assert(m.needsTangents() == true);
	m.setTexture(NORMAL_MAP, nullptr);
	assert(m.needsTangents() == false);

	// Need tangents (disp)
	m.setTexture(DISPLACEMENT_MAP, &t);
	assert(m.needsTangents() == true);
	m.setTexture(DISPLACEMENT_MAP, nullptr);
	assert(m.needsTangents() == false);
}
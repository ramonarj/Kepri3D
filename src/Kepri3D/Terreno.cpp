#include "Terreno.h"

#include "Camera.h"
#include "Scene.h"
#include "Material.h"
#include "ResourceManager.h"

Terreno::Terreno(Material* mat) : cam(nullptr), heightMap(nullptr)
{
	this->mat = mat;
	useEyedir = false;
	patchSize = 320; // TODO numeros magicos
	mat->setInt("patch_size", patchSize);
}

void Terreno::update(GLuint deltaTime)
{
	// Las cosas configurables en tiempo de ejecución son estas 3 de momento
	mat->setInt("use_eyeDir", useEyedir);
	mat->setFloat("elevacion", elevacion);
	mat->setVec3("camFW", cam->forward());
}

void Terreno::setHeightMap(const std::string& texID, float elevacion)
{
	this->heightMap = (Texture*)&ResourceManager::Instance()->getTexture(texID);
	this->elevacion = elevacion;

	mat->setTexture("heightMap", heightMap);
}
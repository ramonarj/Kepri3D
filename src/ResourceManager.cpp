#include "ResourceManager.h"

#include "Texture.h"
#include "Material.h"

#include <fstream>
#include <iostream>

ResourceManager* ResourceManager::instance = nullptr;

bool ResourceManager::loadTexture(std::string fileName, std::string id)
{
	Texture* tex = new Texture();
	try
	{
		tex->load(TEXTURES_PATH + fileName);
		textures[id] = tex;
		return true;
	}

	catch(const std::ios_base::failure& f)
	{
		std::cout << "No se pudo cargar la textura " << "\"" << fileName << "\"" << std::endl;
		//std::cout << f.what() << std::endl;
		// Como no se ha podido cargar la imagen, borramos la textura creada
		delete tex;
		return false;
	}
}

const Texture& ResourceManager::getTexture(std::string id)
{
	// Si no se encuentra la textura dada, se devuelve por defecto una magenta chillón
	if (textures.find(id) != textures.end())
		return *textures[id];
	else
		return *textures["default"];
}

// - - - - - - - - - - - - 

bool ResourceManager::loadMaterial(std::string fileName, std::string id)
{
	try
	{
		// Abrir el archivo
		std::ifstream stream((MATERIALS_PATH + fileName).c_str());
		if (!stream.is_open())
		{
			throw std::ios_base::failure("ResourceManager ERROR: Could not open file");
		}

		// Leer las 3 componentes del material (+ el exponente especular)
		glm::fvec4 ambient{};
		stream >> ambient.x >> ambient.y >> ambient.z >> ambient.w;

		glm::fvec4 diffuse{};
		stream >> diffuse.x >> diffuse.y >> diffuse.z >> diffuse.w;

		glm::fvec4 specular{};
		stream >> specular.x >> specular.y >> specular.z >> specular.w;

		float expF = 0;
		stream >> expF;

		// Cerrar el archivo
		stream.close();

		// Crear el material y añadirlo al diccionario
		materials[id] = new Material(ambient, diffuse, specular, expF);
		return true;
	}

	catch (const std::ios_base::failure& f)
	{
		std::cout << "No se pudo cargar el material " << "\"" << fileName << "\"" << std::endl;
		return false;
	}
}

const Material& ResourceManager::getMaterial(std::string id)
{
	// Si no se encuentra la textura dada, se devuelve por defecto una magenta chillón
	if (materials.find(id) != materials.end())
		return *materials[id];
	else
		return *materials["default"];
}

void ResourceManager::Clean()
{
	// Borrar todas las texturas
	auto itTex = textures.begin();
	while (itTex != textures.end())
	{
		delete itTex->second;
		itTex++;
	}
	textures.clear();

	// Borrar todos los materiales
	auto itMat = materials.begin();
	while (itMat != materials.end())
	{
		delete itMat->second;
		itMat++;
	}
	materials.clear();

	delete instance; 
	instance = nullptr;
}
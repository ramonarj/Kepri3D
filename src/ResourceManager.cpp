#include "ResourceManager.h"

#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "Shader.h"
#include "MeshLoader.h"
#include "Utils.h"

#include <fstream>
#include <iostream>

ResourceManager* ResourceManager::instance = nullptr;

bool ResourceManager::loadMesh(std::string meshName, std::string id)
{
	try
	{
		// Cargamos la malla gracias a MeshLoader
		MeshLoader meshLoader;
		meshLoader.loadOBJ(MESHES_PATH + meshName);
		IndexMesh* m = meshLoader.getMesh();

		// la guardamos en el diccionario
		meshes[id] = m;
		return true;
	}

	catch (const std::ios_base::failure& f)
	{
		std::cout << "No se pudo cargar la malla " << "\"" << meshName << "\"" << std::endl;
		return false;
	}
}

const Mesh& ResourceManager::getMesh(std::string id)
{
	// Si no se encuentra la malla dada, se devuelve por defecto... TODO
	if (meshes.find(id) != meshes.end())
		return *meshes[id];
	else
		return *meshes["default"];
}

// - - - - - - - - - - - - 

bool ResourceManager::loadTexture(std::string textureName, std::string id)
{
	Texture* tex = new Texture();
	try
	{
		tex->load(TEXTURES_PATH + textureName);
		textures[id] = tex;
		return true;
	}

	catch(const std::ios_base::failure& f)
	{
		std::cout << "No se pudo cargar la textura " << "\"" << textureName << "\"" << std::endl;
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

bool ResourceManager::loadMaterial(std::string materialName, std::string id)
{
	try
	{
		// Abrir el archivo
		std::ifstream stream((MATERIALS_PATH + materialName).c_str());
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
		std::cout << "No se pudo cargar el material " << "\"" << materialName << "\"" << std::endl;
		return false;
	}
}

const Material& ResourceManager::getMaterial(std::string id)
{
	// Si no se encuentra el material dado, se devuelve el predeterminado
	if (materials.find(id) != materials.end())
		return *materials[id];
	else
		return *materials["default"];
}

// - - - - - - - - - - - - 

bool ResourceManager::loadShader(std::string vertexName, std::string fragmentName, std::string id)
{
	try
	{
		// Leer los shaders de archivo
		std::string VSprogram = FileToString((SHADERS_PATH + vertexName).c_str());
		std::string FSprogram = FileToString((SHADERS_PATH + fragmentName).c_str());
		std::string GSprogram = FileToString((SHADERS_PATH + "lineas.geom").c_str());

		// Crear el shader program y añadirlo al diccionario
		Shader* sh = new Shader();
		//sh->load(VSprogram.c_str(), FSprogram.c_str());
		sh->load(GL_VERTEX_SHADER, VSprogram.c_str());
		sh->load(GL_FRAGMENT_SHADER, FSprogram.c_str());
		sh->load(GL_GEOMETRY_SHADER, GSprogram.c_str());
		// Compilarlo
		sh->link();

		shaders[id] = sh;
		return true;
	}

	catch (const std::ios_base::failure& f)
	{
		std::cout << f.what();
		return false;
	}
}

const Shader& ResourceManager::getShader(std::string id)
{
	// Si no se encuentra el shader especificado, se devuelve el predeterminado
	if (shaders.find(id) != shaders.end())
		return *shaders[id];
	else
		return *shaders["default"];
}

// - - - - - - - - - - - - 

void ResourceManager::Clean()
{
	// Borrar todas las mallas
	//CleanMap(meshes);

	// Borrar todas las texturas
	CleanMap(textures);

	// Borrar todos los materiales
	CleanMap(materials);

	// Borrar todos los shaders
	CleanMap(shaders);

	delete instance; 
	instance = nullptr;
}
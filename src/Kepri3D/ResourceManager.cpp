#include "ResourceManager.h"

#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "Shader.h"
#include "MeshLoader.h"
#include "Utils.h"

#include <fstream>
#include <iostream>

#ifdef _WIN32
	#include <Windows.h>
	#include <regex>
#endif

ResourceManager* ResourceManager::instance = nullptr;
std::string ResourceManager::ASSETS_PATH;

std::string ResourceManager::TEXTURES_PATH;
std::string ResourceManager::MESHES_PATH;
std::string ResourceManager::MATERIALS_PATH;
std::string ResourceManager::SHADERS_PATH;
std::string ResourceManager::COMPOSITES_PATH;

bool ResourceManager::loadMesh(const std::string& meshName, const std::string& id, float scale)
{
	std::cout << "Loading mesh " << meshName << std::endl;
	try
	{
		// Cargamos la malla gracias a MeshLoader
		MeshLoader meshLoader;
		meshLoader.loadOBJ(MESHES_PATH + meshName, scale);
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

const Mesh& ResourceManager::getMesh(const std::string& id)
{
	// Si no se encuentra la malla dada, se devuelve por defecto... TODO
	if (meshes.find(id) != meshes.end())
		return *meshes[id];
	else
		return *meshes["default"];
}

// - - - - - - - - - - - - 

bool ResourceManager::loadTexture(const std::string& textureName, const std::string& id, GLubyte alpha, GLint internalFormat)
{
	Texture* tex = new Texture();
	try
	{
		tex->load(TEXTURES_PATH + textureName, alpha, internalFormat);
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

bool ResourceManager::loadTexture(const std::string& textureName, const std::string& id, const glm::ivec3& colorTrans)
{
	Texture* tex = new Texture();
	try
	{
		tex->load(TEXTURES_PATH + textureName, colorTrans);
		textures[id] = tex;
		return true;
	}

	catch (const std::ios_base::failure& f)
	{
		std::cout << "No se pudo cargar la textura " << "\"" << textureName << "\"" << std::endl;
		//std::cout << f.what() << std::endl;
		// Como no se ha podido cargar la imagen, borramos la textura creada
		delete tex;
		return false;
	}
}

const Texture& ResourceManager::getTexture(const std::string& id)
{
	// Si no se encuentra la textura dada, se devuelve por defecto una magenta chillón
	if (textures.find(id) != textures.end())
		return *textures[id];
	else
		return *textures["default"];
}

// - - - - - - - - - - - - 

bool ResourceManager::loadMaterial(const std::string& materialName, const std::string& id)
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

		glm::fvec4 emission{};
		stream >> emission.x >> emission.y >> emission.z >> emission.w;

		float expF = 0;
		stream >> expF;

		// Cerrar el archivo
		stream.close();

		// Crear el material y añadirlo al diccionario
		materials[id] = new Material(ambient, diffuse, specular, emission, expF);
		return true;
	}

	catch (const std::ios_base::failure& f)
	{
		std::cout << "No se pudo cargar el material " << "\"" << materialName << "\"" << std::endl;
		return false;
	}
}

const Material ResourceManager::getMaterial(const std::string& id)
{
	// Si no se encuentra el material dado, se devuelve el predeterminado
	if (materials.find(id) != materials.end())
		return *materials[id];
	else
		return *materials["default"];
}

// - - - - - - - - - - - - 

bool ResourceManager::loadShader(const std::string& vertexName, const std::string& geometryName, 
	const std::string& fragmentName, const std::string& id)
{
	return loadShader(vertexName, "", "", geometryName, fragmentName, id);
}

bool ResourceManager::loadShader(const std::string& vertexName, const std::string& tesControlName, 
	const std::string& tesEvalName, const std::string& geometryName, const std::string& fragmentName, const std::string& id)
{
	try
	{
		Shader* sh = new Shader();

		// Leer los shaders de archivo y cargarlos en el shader program
		if (vertexName != "")
		{
			std::string VSprogram = FileToString((SHADERS_PATH + vertexName).c_str());
			sh->load(GL_VERTEX_SHADER, VSprogram.c_str());
		}
		if (tesControlName != "")
		{
			std::string TCSprogram = FileToString((SHADERS_PATH + tesControlName).c_str());
			sh->load(GL_TESS_CONTROL_SHADER, TCSprogram.c_str());
		}
		if (tesEvalName != "")
		{
			std::string TESprogram = FileToString((SHADERS_PATH + tesEvalName).c_str());
			sh->load(GL_TESS_EVALUATION_SHADER, TESprogram.c_str());
		}
		if (geometryName != "")
		{
			std::string GSprogram = FileToString((SHADERS_PATH + geometryName).c_str());
			sh->load(GL_GEOMETRY_SHADER, GSprogram.c_str());
		}
		if (fragmentName != "")
		{
			std::string FSprogram = FileToString((SHADERS_PATH + fragmentName).c_str());
			sh->load(GL_FRAGMENT_SHADER, FSprogram.c_str());
		}

		// Compilarlo y añadirlo al diccionario
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

const Shader& ResourceManager::getShader(const std::string& id)
{
	// Si no se encuentra el shader especificado, se devuelve el predeterminado
	if (shaders.find(id) != shaders.end())
		return *shaders[id];
	else
		return *shaders["default"];
}

// - - - - - - - - - - - - 

// Muy parecido a 'loadShader'
bool ResourceManager::loadComposite(const std::string& compositeName, const std::string& id)
{
	try
	{
		Shader* sh = new Shader();

		// Vertex shader único para todos los efectos composite
		std::string VSprogram = FileToString((COMPOSITES_PATH + "composite.vert").c_str());
		sh->load(GL_VERTEX_SHADER, VSprogram.c_str());

		// Fragment shader especificado
		if (compositeName != "")
		{
			std::string FSprogram = FileToString((COMPOSITES_PATH + compositeName).c_str());
			sh->load(GL_FRAGMENT_SHADER, FSprogram.c_str());
		}

		// Compilarlo y añadirlo al diccionario
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

const Shader& ResourceManager::getComposite(const std::string& id)
{
	return getShader(id);
}

// - - - - - - - - - - - - - - - - - - 

bool ResourceManager::loadCubemapTexture(std::vector<std::string> facesNames, const std::string& id)
{
	CubemapTexture* tex = new CubemapTexture();
	try
	{
		// Añadirle la ruta adecuada
		for (int i = 0; i < facesNames.size(); i++)
			facesNames[i] = TEXTURES_PATH + facesNames[i];
		// Cargarlo de archivo
		tex->load(facesNames);
		textures[id] = tex;
		return true;
	}

	catch (const std::ios_base::failure& f)
	{
		std::cout << "No se pudo cargar la textura cubemap " << "\"" << id << "\"" << std::endl;
		//std::cout << f.what() << std::endl;
		// Como no se ha podido cargar la imagen, borramos la textura creada
		delete tex;
		return false;
	}
}

void ResourceManager::enableMipmaps(bool b)
{
	// Actualizar los parámetros de todas las texturas para que usen/no mipmaps
	auto it = textures.begin();
	while (it != textures.end())
	{
		it->second->useMipmaps(b);
		it++;
	}
}



void ResourceManager::setAssetsPath()
{
#ifdef _WIN32
	// Ruta absoluta del ejecutable
	wchar_t buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	// Convertirlo a string normal
	std::wstring ws(buffer);
	std::string str(ws.begin(), ws.end());
	// Quitarle el nombre del ejecutable
	unsigned int i = str.length() - 1;
	while (str[i] != '\\')
		str.erase(i--);

	// Actualizar las rutas (absolutas)
	ASSETS_PATH = str + "assets\\";

	TEXTURES_PATH = ASSETS_PATH + "textures\\";
	MESHES_PATH = ASSETS_PATH + "meshes\\";
	MATERIALS_PATH = ASSETS_PATH + "materials\\";
	SHADERS_PATH = ASSETS_PATH + "shaders\\";
	COMPOSITES_PATH = SHADERS_PATH + "postprocess\\";
#endif
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
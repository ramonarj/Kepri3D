#include "ResourceManager.h"

#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "Shader.h"
#include "MeshLoader.h"
#include "Utils.h"
#include "Audio.h"

#include <fstream>
#include <iostream>

#ifdef _WIN32
	#include <Windows.h>
#endif

ResourceManager* ResourceManager::instance = nullptr;
std::string ResourceManager::ASSETS_PATH;

std::string ResourceManager::TEXTURES_PATH;
std::string ResourceManager::MESHES_PATH;
std::string ResourceManager::MATERIALS_PATH;
std::string ResourceManager::SHADERS_PATH;
std::string ResourceManager::AUDIO_PATH;
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

		// Le calculamos el Bounding Volume
		m->calculateVolume();

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

Mesh* ResourceManager::getMesh(const std::string& id)
{
	// Si no se encuentra la malla dada, se devuelve por defecto... TODO
	if (meshes.find(id) != meshes.end())
		return meshes[id];
	else
		return meshes["default"];
}

// - - - - - - - - - - - - 

bool ResourceManager::loadTexture(const std::string& textureName, const std::string& id, GLubyte alpha, GLint internalFormat)
{
	Texture* tex = new Texture();
	try
	{
		if (tex->load(TEXTURES_PATH + textureName, alpha, internalFormat))
		{
			textures[id] = tex;
			return true;
		}
	}
	catch(const std::ios_base::failure& f)
	{
		std::cout << "No se pudo cargar la textura " << "\"" << textureName << "\"" << std::endl;
		//std::cout << f.what() << std::endl;
	}
	// Como no se ha podido cargar la imagen, borramos la textura creada
	delete tex;
	return false;
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

Texture* ResourceManager::getTexture(const std::string& id)
{
	// Si no se encuentra la textura dada, se devuelve por defecto una magenta chill�n
	if (textures.find(id) != textures.end())
		return textures[id];
	else
		return textures["default"];
}

// - - - - - - - - - - - - 

bool ResourceManager::loadMaterial(const std::string& materialName, const std::string& id, const std::string& shaderId)
{
	try
	{
		// Abrir el archivo
		std::ifstream stream((MATERIALS_PATH + materialName).c_str());
		if (!stream.is_open())
		{
			throw std::ios_base::failure("ResourceManager ERROR: Could not open file");
		}

		// Leer las 4 componentes del material (+ el exponente especular)
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

		// Leer todos los mapas
		std::string diffuseMap, secondaryMap, specularMap, normalMap, parallaxMap, emissionMap, skybox, reflectionMap;
		diffuseMap = secondaryMap = specularMap = normalMap = parallaxMap = emissionMap = skybox = reflectionMap = "";
		std::vector<std::pair<std::string, std::string>> mapasExtra; int numMapasExtra = 0;
		for(int i = 0; i < 5; i++) // TODO: 5??
		{
			std::string tipo;
			stream >> tipo;
			if (tipo == "map_Kd")
				stream >> diffuseMap;
			else if (tipo == "map_Kd2")
				stream >> secondaryMap;
			else if (tipo == "map_Ks")
				stream >> specularMap;
			else if (tipo == "map_normal")
				stream >> normalMap;
			else if (tipo == "map_disp")
				stream >> parallaxMap;
			else if (tipo == "map_Ke")
				stream >> emissionMap;
			else if (tipo == "skybox")
				stream >> skybox;
			else if (tipo == "map_reflection")
				stream >> reflectionMap;
			// Mapa definido por el usuario
			else 
			{
				mapasExtra.push_back(std::pair<std::string, std::string>());
				mapasExtra[numMapasExtra].first = tipo;
				stream >> mapasExtra[numMapasExtra].second;
				numMapasExtra++;
			}

			// Etiqueta inv�lida
			//else if(tipo != "")
			//{
			//	std::cout << "WARNING: etiqueta '" << tipo << "' no reconocida en \"" << materialName << "\"" << std::endl;
			//	stream >> tipo;
			//}
		}

		// Cerrar el archivo
		stream.close();

		// Crear el material y darle los valores
		Material* material = new Material(ambient, diffuse, specular, emission, expF);

		// A�adirle las texturas que usar�
		if (diffuseMap != "")
		{
			// No existe; intentamos cargarla nosotros (probando extensi�n .jpg, .png y .bmp)
			Texture* t = getTexture(diffuseMap);
			if(t->getWidth() == 1)
			{
				if (!loadTexture(diffuseMap + ".jpg", diffuseMap))
					if (!loadTexture(diffuseMap + ".png", diffuseMap))
						loadTexture(diffuseMap + ".bmp", diffuseMap);
			}
			material->setTexture(DIFFUSE_MAP, getTexture(diffuseMap));
		}
		if (secondaryMap != "")
			material->setTexture(SECONDARY_MAP, getTexture(secondaryMap));
		if (specularMap != "")
			material->setTexture(SPECULAR_MAP, getTexture(specularMap));
		if (normalMap != "")
			material->setTexture(NORMAL_MAP, getTexture(normalMap));
		if (parallaxMap != "")
			material->setTexture(DISPLACEMENT_MAP, getTexture(parallaxMap));
		if (reflectionMap != "")
			material->setTexture(REFLECTION_MAP, getTexture(reflectionMap));
		if (skybox != "")
			material->setTexture(SKYBOX_MAP, getTexture(skybox));
		if (emissionMap != "")
			material->setTexture(EMISSION_MAP, getTexture(emissionMap));

		// Mapas extra
		for(int i = 0; i < numMapasExtra; i++)
		{
			material->setTexture(mapasExtra[i].first, getTexture(mapasExtra[i].second));
		}

		// Indicarle qu� shader usa
		material->setShader(getShader(shaderId));

		// Por �ltimo, a�adirlo al diccionario
		materials[id] = material;

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
			FSprogram = sh->preprocess(FSprogram.c_str());
			sh->load(GL_FRAGMENT_SHADER, FSprogram.c_str());
		}

		// Compilarlo y a�adirlo al diccionario
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

Shader* ResourceManager::getShader(const std::string& id)
{
	// Si no se encuentra el shader especificado, se devuelve el predeterminado
	if (shaders.find(id) != shaders.end())
		return shaders[id];
	else
		return shaders["default"];
}

// - - - - - - - - - - - - 

// Muy parecido a 'loadShader'
bool ResourceManager::loadComposite(const std::string& compositeName, const std::string& id)
{
	try
	{
		Shader* sh = new Shader();

		// Vertex shader �nico para todos los efectos composite
		std::string VSprogram = FileToString((COMPOSITES_PATH + "composite.vert").c_str());
		sh->load(GL_VERTEX_SHADER, VSprogram.c_str());

		// Fragment shader especificado
		if (compositeName != "")
		{
			std::string FSprogram = FileToString((COMPOSITES_PATH + compositeName).c_str());
			FSprogram = sh->preprocess(FSprogram.c_str());
			sh->load(GL_FRAGMENT_SHADER, FSprogram.c_str());
		}

		// Compilarlo y a�adirlo al diccionario
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

Shader* ResourceManager::getComposite(const std::string& id)
{
	// capa de cebolla
	return getShader(id);
}

// - - - - - - - - - - - - - - - - - - 

bool ResourceManager::loadCubemapTexture(std::vector<std::string> facesNames, const std::string& id)
{
	CubemapTexture* tex = new CubemapTexture();
	try
	{
		// A�adirle la ruta adecuada
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

// - - - - - - - - 
bool ResourceManager::loadAudio(const std::string& fileName, const std::string& id)
{
	Audio* audio = new Audio(AUDIO_PATH + fileName, Audio::WAV);
	audios[id] = audio;
	return true;
}

Audio* ResourceManager::getAudio(const std::string& id)
{
	// Si no se encuentra el shader especificado, se devuelve el predeterminado
	if (audios.find(id) != audios.end())
		return audios[id];
	else
		return audios["default"];
}

// - - - - - - - - - - 

void ResourceManager::enableMipmaps(bool b)
{
	// Actualizar los par�metros de todas las texturas para que usen/no mipmaps
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
	AUDIO_PATH = ASSETS_PATH + "audio\\";
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

	// Borrar todos los audios
	CleanMap(audios);

	delete instance; 
	instance = nullptr;
}
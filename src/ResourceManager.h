#ifndef __TEXTURE_MANAGER__
#define __TEXTURE_MANAGER__

#include <map>
#include <string>
class Texture;
class Material;

const std::string TEXTURES_PATH = "..\\bin\\assets\\";
const std::string MATERIALS_PATH = "..\\bin\\assets\\materials\\";

class ResourceManager
{
public:
	/* Acceso al TextureManager */
	static ResourceManager* Instance()
	{
		if (instance == nullptr)
		{
			instance = new ResourceManager();

			// Carga de textura y material por defecto
			instance->loadTexture("default.bmp", "default");
			instance->loadMaterial("default.material", "default");
		}
			
		return instance;
	}

	/* Carga una textura de archivo y le asigna el ID dado */
	bool loadTexture(std::string fileName, std::string id);

	/* Carga un material de archivo y le asigna el ID dado */
	bool loadMaterial(std::string fileName, std::string id);

	/* Devuelve una textura dada */
	const Texture& getTexture(std::string id);

	/* Devuelve un material dado */
	const Material& getMaterial(std::string id);

	/* Limpia la instancia; debe llamarse explícitamente */
	void Clean();
private:
	ResourceManager() {}
	static ResourceManager* instance;

	/* Diccionario de texturas accesibles por su nombre (string) */
	std::map<std::string, Texture*> textures;

	/* Diccionario de materiales accesibles por su nombre (string) */
	std::map<std::string, Material*> materials;
};

#endif
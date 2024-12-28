#ifndef __TEXTURE_MANAGER__
#define __TEXTURE_MANAGER__

#include <map>
#include <string>

class Mesh;
class Texture;
class Material;
class Shader;


const std::string MESHES_PATH = "..\\bin\\assets\\meshes\\";
const std::string TEXTURES_PATH = "..\\bin\\assets\\textures\\";
const std::string MATERIALS_PATH = "..\\bin\\assets\\materials\\";
const std::string SHADERS_PATH = "..\\bin\\assets\\shaders\\";

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
			instance->loadShader("default.vert", "default.frag", "default");
		}
			
		return instance;
	}

	// Carga de recursos
	/* Carga una malla de archivo y le asigna el ID dado */
	bool loadMesh(std::string meshName, std::string id);

	/* Carga una textura de archivo y le asigna el ID dado */
	bool loadTexture(std::string textureName, std::string id);

	/* Carga un material de archivo y le asigna el ID dado */
	bool loadMaterial(std::string materialName, std::string id);

	/* Carga un shader de archivo y le asigna el ID dado */
	bool loadShader(std::string vertexName, std::string fragmentName, std::string id);

	// Proporcionar los recursos
	/* Devuelve una textura dada */
	const Mesh& getMesh(std::string id);

	/* Devuelve una textura dada */
	const Texture& getTexture(std::string id);

	/* Devuelve un material dado */
	const Material& getMaterial(std::string id);

	/* Devuelve un shader dado */
	const Shader& getShader(std::string id);

	// Otros métodos
	void enableMipmaps(bool b);

	// Limpieza
	/* Limpia la instancia; debe llamarse explícitamente */
	void Clean();
private:
	ResourceManager() {}
	static ResourceManager* instance;

	/* Diccionario de mallas accesibles por su nombre (string) */
	std::map<std::string, Mesh*> meshes;

	/* Diccionario de texturas accesibles por su nombre (string) */
	std::map<std::string, Texture*> textures;

	/* Diccionario de materiales accesibles por su nombre (string) */
	std::map<std::string, Material*> materials;

	/* Diccionario de shaders accesibles por su nombre (string) */
	std::map<std::string, Shader*> shaders;
};

#endif
#ifndef __TEXTURE_MANAGER__
#define __TEXTURE_MANAGER__

#include <map>
#include <string>
#include <vector>
#include <glew.h>
#include <glm.hpp>

class Mesh;
class Texture;
class Material;
class Shader;


const std::string MESHES_PATH = "..\\..\\bin\\assets\\meshes\\";
const std::string TEXTURES_PATH = "..\\..\\bin\\assets\\textures\\";
const std::string MATERIALS_PATH = "..\\..\\bin\\assets\\materials\\";
const std::string SHADERS_PATH = "..\\..\\bin\\assets\\shaders\\";
const std::string COMPOSITES_PATH = "..\\..\\bin\\assets\\shaders\\postprocess\\";

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
			instance->loadShader("default.vert", "default.geom", "default.frag", "default");
			instance->loadComposite("defaultComposite.frag", "defaultComposite");

			// Otros shaders necesarios (skybox, part�culas, normales)
			instance->loadShader("skybox.vert", "", "skybox.frag", "skybox");
			instance->loadShader("particle.vert", "", "particle.frag", "particle");
			instance->loadShader("normals.vert", "normals.geom", "normals.frag", "normals");
		}
			
		return instance;
	}

	// Carga de recursos
	/* Carga una malla de archivo y le asigna el ID dado */
	bool loadMesh(const std::string& meshName, const std::string& id);

	/* Carga una textura de archivo y le asigna el ID dado */
	bool loadTexture(const std::string& textureName, const std::string& id, GLubyte alpha = 255);
	bool loadTexture(const std::string& textureName, const std::string& id, const glm::ivec3& colorTrans);

	/* Carga un material de archivo y le asigna el ID dado */
	bool loadMaterial(const std::string& materialName, const std::string& id);

	/* Carga un shader de archivo y le asigna el ID dado */
	bool loadShader(const std::string& vertexName, const std::string& geometryName, 
		const std::string& fragmentName, const std::string& id);

	/* Carga un composite de archivo y le asigna el ID dado */
	bool loadComposite(const std::string& compositeName, const std::string& id);

	/* Carga una textura de tipo Cubemap con los archivos dados, y le asigna el ID dado */
	bool loadCubemapTexture(std::vector<std::string> facesNames, const std::string& id);

	// Proporcionar los recursos
	/* Devuelve una textura dada */
	const Mesh& getMesh(const std::string& id);

	/* Devuelve una textura dada */
	const Texture& getTexture(const std::string& id);

	/* Devuelve un material dado */
	const Material& getMaterial(const std::string& id);

	/* Devuelve un shader dado */
	const Shader& getShader(const std::string& id);

	/* Devuelve un composite dado */
	const Shader& getComposite(const std::string& id);


	// Otros m�todos
	void enableMipmaps(bool b);

	// Limpieza
	/* Limpia la instancia; debe llamarse expl�citamente */
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
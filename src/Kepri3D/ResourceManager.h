#ifndef __TEXTURE_MANAGER__
#define __TEXTURE_MANAGER__

#include <map>
#include <string>
#include <vector>
#include <glew.h>
#include <glm.hpp>
#include "Texture.h"

class Mesh;
class Texture;
class Material;
class Shader;



class ResourceManager
{
public:
	// Rutas de recursos
	static std::string ASSETS_PATH;

	static std::string TEXTURES_PATH;
	static std::string MESHES_PATH;
	static std::string MATERIALS_PATH;
	static std::string SHADERS_PATH;
	static std::string COMPOSITES_PATH;

	/* Acceso al TextureManager */
	static ResourceManager* Instance()
	{
		if (instance == nullptr)
		{
			instance = new ResourceManager();

			instance->setAssetsPath();

			// Otros shaders necesarios (skybox, partículas, agua, normales...)
			instance->loadShader("shadows.vert", "", "shadows.frag", "shadows");
			instance->loadShader("shadows_point.vert", "shadows_point.geom", "shadows_point.frag", "shadows_point");
			instance->loadShader("lights.vert", "", "lights.frag", "lights");
			instance->loadShader("skybox.vert", "", "skybox.frag", "skybox");
			instance->loadShader("billboard.vert", "", "billboard.frag", "billboard");
			instance->loadShader("particle.vert", "", "billboard.frag", "particle");
			instance->loadShader("agua.vert", "", "agua.frag", "agua");
			instance->loadShader("UI.vert", "", "UI.frag", "UI");
			instance->loadShader("normals.vert", "normals.geom", "normals.frag", "normals");

			// Carga de textura y material por defecto
			instance->loadTexture("default.bmp", "default");
			instance->loadMaterial("default.material", "default");
			instance->loadShader("default.vert", "default.geom", "default.frag", "default");
			instance->loadComposite("defaultComposite.frag", "defaultComposite");
		}
			
		return instance;
	}

	// Carga de recursos
	/* Carga una malla de archivo y le asigna el ID dado */
	bool loadMesh(const std::string& meshName, const std::string& id, float scale = 1.0f);

	/* Carga una textura de archivo y le asigna el ID dado */
	bool loadTexture(const std::string& textureName, const std::string& id, 
		GLubyte alpha = 255, GLint internalFormat = COLOR_SPACE);
	bool loadTexture(const std::string& textureName, const std::string& id, const glm::ivec3& colorTrans);

	/* Carga un material de archivo, especifica el shader que usará, y le asigna el ID dado */
	bool loadMaterial(const std::string& materialName, const std::string& id, const std::string& shaderId = "lights");

	/* Carga un shader de archivo y le asigna el ID dado */
	bool loadShader(const std::string& vertexName, const std::string& geometryName, const std::string& tesControlName,
		const std::string& tesEvalName, const std::string& fragmentName, const std::string& id);
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
	const Material getMaterial(const std::string& id);

	/* Devuelve un shader dado */
	const Shader& getShader(const std::string& id);

	/* Devuelve un composite dado */
	const Shader& getComposite(const std::string& id);


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

	//
	void setAssetsPath();
};

#endif
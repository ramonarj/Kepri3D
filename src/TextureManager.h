#ifndef __TEXTURE_MANAGER__
#define __TEXTURE_MANAGER__

#include <map>
#include <string>
class Texture;

const std::string TEXTURES_PATH = "..\\bin\\assets\\";

class TextureManager
{
public:
	/* Acceso al TextureManager */
	static TextureManager* Instance()
	{
		if (instance == nullptr)
			instance = new TextureManager();
		return instance;
	}

	/* Carga una textura de archivo y le asigna el ID dado */
	bool load(std::string fileName, std::string id);

	/* Devuelve una textura dada */
	const Texture& getTexture(std::string id);

	/* Limpia la instancia; debe llamarse explícitamente */
	void Clean();
private:
	TextureManager() {}
	static TextureManager* instance;

	/* Diccionario de texturas accesibles por su nombre (string) */
	std::map<std::string, Texture*> textures;
};

#endif
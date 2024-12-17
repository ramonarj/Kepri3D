#include "TextureManager.h"

#include "Texture.h"
#include <fstream>
#include <iostream>

TextureManager* TextureManager::instance = nullptr;

bool TextureManager::load(std::string fileName, std::string id)
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
		// Como no se ha podido cargar la imagen, borramos la textura creada
		delete tex;
		return false;
	}
}

const Texture& TextureManager::getTexture(std::string id)
{
	return *textures[id];
}

void TextureManager::Clean()
{
	// Borrar todas las texturas
	auto it = textures.begin();
	while (it != textures.end())
	{
		delete it->second;
		it++;
	}
	textures.clear();

	delete instance; 
	instance = nullptr;
}
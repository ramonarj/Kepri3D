#include "Utils.h"

#include <fstream>

std::string FileToString(const char* fileName)
{
	// Abrir el archivo
	std::ifstream stream(fileName);
	if (!stream.is_open())
	{
		throw std::ios_base::failure("No se pudo abrir el archivo '" + std::string(fileName) + "'");
	}

	// Leer todo el archivo línea a línea
	std::string linea;
	std::string fileContent = "";
	while (!stream.eof())
	{
		std::getline(stream, linea);
		fileContent = fileContent + linea + '\n';
	}

	// Cerrar el archivo
	stream.close();

	// Devolver el texto
	return fileContent;
}

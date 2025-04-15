#include "Utils.h"

#include <fstream>
#include <sstream>
#include <iomanip>

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

void PrintVector(const glm::vec3& v)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(2)
		<< "(" << v.x << "," << v.y << "," << v.z << ")";

	std::cout << stream.str();
}

glm::vec3 Kepri::lerp(const glm::vec3& a, const glm::vec3& b, float l)
{
	if (l < 0) l = 0;
	else if (l > 1) l = 1;
	return a + (b - a) * l;
}
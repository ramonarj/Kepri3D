#include "MeshLoader.h"

#include "Mesh.h"

#include <fstream>
#include <iostream>

using namespace glm;


void MeshLoader::loadOBJ(const std::string& filename)
{
	// A priori no podemos saber cuántos vértices tendrá la malla, por eso usamos un std::vector
	// que nos permita ampliar el tamaño todo lo que se quiera con 'push_back'
	//std::vector<glm::dvec3> vertices;

	// Abrir el archivo
	std::ifstream stream(filename.c_str());
	if (!stream.is_open())
	{
		throw std::ios_base::failure("MeshLoader ERROR: Could not open file");
	}

	// Crear la malla
	mesh = new IndexMesh();
	mesh->type = GL_TRIANGLES;

	// Llegamos a los vértices
	bool stopRead = false;
	std::string texto;
	while(!stopRead)
	{
		// 'Marcador' para poder volver a esa posición si nos pasamos
		std::streampos inicioVert = stream.tellg();

		std::getline(stream, texto);
		if (texto[0] == 'v' && texto[1] == ' ')
		{
			//hemos llegado
			stopRead = true;
			stream.seekg(inicioVert);
		}
	}

	/* 1) Leer los vértices */
	readVertices(stream);

	// Crear los arrays de la propia malla
	mesh->numVertices = vertices.size();
	mesh->vertices = new glm::dvec3[mesh->numVertices];
	mesh->texCoords = new glm::dvec2[mesh->numVertices];
	mesh->normales = new glm::dvec3[mesh->numVertices];
	for (int i = 0; i < mesh->numVertices; i++)
		mesh->vertices[i] = this->vertices[i];
	//std::cout << "Num. vertices: " << mesh->numVertices << std::endl;

	/* 2) Leer las normales */
	readNormals(stream);

	/* 3) Leer las coordenadas de textura */
	readTexCoords(stream);

	/* 4) Leer los triángulos y a la vez rellenar los datos de la malla */
	readFaces(stream);
	mesh->numIndices = indices.size() * 3;
	mesh->indices = new GLuint[mesh->numIndices];
	for (int i = 0; i < mesh->numIndices; i+=3)
	{
		glm::ivec3 tri = indices[(i / 3) + (i % 3)];

		mesh->indices[i] = tri.x;
		mesh->indices[i+1] = tri.y;
		mesh->indices[i+2] = tri.z;
	}

	// No sería necesario
	mesh->SetNormals();


	//for(int i = 0; i < vertices.size(); i++)
	//	std::cout << vertices[i].x << ", " << vertices[i].y << ", " << vertices[i].z << std::endl;
	//for(int i = 0; i < normales.size(); i++)
	//	std::cout << normales[i].x << ", " << normales[i].y << ", " << normales[i].z << std::endl;
	//for (int i = 0; i < texCoords.size(); i++)
	//	std::cout << texCoords[i].x << ", " << texCoords[i].y << std::endl;


	// Cerrar el archivo
	stream.close();
}

void MeshLoader::readVertices(std::ifstream& stream)
{
	// Leer todos los vértices
	int i = 0;
	std::string texto = "v";
	while (texto == "v")
	{
		// Comprobar que sigue siendo un vértice
		stream >> texto;
		if (texto != "v")
			continue;

		// Leer sus componentes
		double vX, vY, vZ;
		stream >> vX >> vY >> vZ;

		// Añadirlo a la lista
		vertices.push_back({ vX, vY, vZ });
		i++;
	}

	// "v" "n"
	stream.unget();
	stream.unget();
}

// Idéntico al método anterior
void MeshLoader::readNormals(std::ifstream& stream)
{
	// Leer todos los vértices
	int i = 0;
	std::string texto = "vn";
	while (texto == "vn")
	{
		// Comprobar que sigue siendo un vector normal
		stream >> texto;
		if (texto != "vn")
			continue;

		// Leer sus componentes
		double nX, nY, nZ;
		stream >> nX >> nY >> nZ;

		// Añadirlo a la lista de normales
		normales.push_back({ nX, nY, nZ });
		i++;
	}

	// "v" "t"
	stream.unget();
	stream.unget();
}


void MeshLoader::readTexCoords(std::ifstream& stream)
{
	// Leer todos los vértices
	int i = 0;
	std::string texto = "vt";
	while (texto == "vt")
	{
		// Comprobar que sigue siendo una coordenada de textura
		stream >> texto;
		if (texto != "vt")
			continue;

		// Leer sus componentes
		double tX, tY;
		stream >> tX >> tY;

		// Añadirlo a la lista de coordenadas
		texCoords.push_back({ tX, tY });
		i++;
	}

	// 's' '0'
	std::getline(stream, texto);
}

void MeshLoader::readFaces(std::ifstream& stream)
{
	// Leer todos los triángulos
	int i = 0;
	std::string texto = "f";
	while (!stream.eof() && texto == "f")
	{
		// Comprobar que sigue siendo un triángulo
		stream >> texto;
		if (texto != "f")
			continue;

		int v1, v2, v3;
		int vt, vn;

		// Primer vértice
		stream >> v1;
		stream.get();
		stream >> vt;
		stream.get();
		stream >> vn;
		stream.get();

		mesh->texCoords[v1 - 1] = texCoords[vt - 1];
		mesh->normales[v1 - 1] = normales[vn - 1];

		// Segundo vértice
		stream >> v2;
		stream.get();
		stream >> vt;
		stream.get();
		stream >> vn;
		stream.get();

		mesh->texCoords[v2 - 1] = texCoords[vt - 1];
		mesh->normales[v2 - 1] = normales[vn - 1];

		// Tercer vértice
		stream >> v3;
		stream.get();
		stream >> vt;
		stream.get();
		stream >> vn;
		stream.get();

		mesh->texCoords[v3 - 1] = texCoords[vt - 1];
		mesh->normales[v3 - 1] = normales[vn - 1];

		// Añadir el triángulo
		indices.push_back({ v1 - 1 , v2 - 1, v3 - 1 });
		i++;
	}
}
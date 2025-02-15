#include "MeshLoader.h"

#include "Mesh.h"

#include <fstream>
#include <iostream>

using namespace glm;


void MeshLoader::loadOBJ(const std::string& filename, float scale)
{
	this->scale = scale;

	// Abrir el archivo (lectura porque solo vamos a leer)
	std::ifstream stream(filename.c_str(), std::ios::in);
	if (!stream.is_open())
	{ throw std::ios_base::failure("MeshLoader ERROR: Could not open file"); }

	// 1) Vuelta de reconocimiento para ver en qué línea empieza cada array de datos
	Reconocimiento(stream);

	// 2) Crear la malla y los arrays con el tamaño correspondiente
	mesh = new IndexMesh();
	mesh->type = GL_TRIANGLES;
	mesh->numVertices = m_vertices.num;

	// Volver a la línea de los vértices
	stream.seekg(0, std::ios::beg);
	lineNo = 1;
	getTo("v ", stream);

	/* a) Leer los vértices */
	mesh->vertices = new glm::dvec3[mesh->numVertices];
	readVertices(stream);

	/* b) Leer las normales */
	mesh->normales = new glm::dvec3[mesh->numVertices];
	readNormals(stream);

	/* c) Leer las coordenadas de textura */
	mesh->texCoords = new glm::dvec2[mesh->numVertices];
	readTexCoords(stream);

	// Nos saltamos el parámetro smooth
	std::string texto;
	std::getline(stream, texto);
	std::getline(stream, texto); // ???
	lineNo++;

	/* d) Leer los triángulos */
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

	// Cerrar el archivo
	stream.close();
}

void MeshLoader::Reconocimiento(std::ifstream& stream)
{
	// Nos colocamos en la primera línea y vamos avanzando, guardando
	// cuántos vértices, normales, texCoords, etc. hay, y en qué línea empieza cada lista
	lineNo = 1;

	getTo("v ", stream);
	m_vertices.line = lineNo;

	getTo("vn", stream);
	m_vertices.num = lineNo - m_vertices.line;
	m_normales.line = lineNo;

	getTo("vt", stream);
	m_normales.num = lineNo - m_normales.line;
	m_texCoods.line = lineNo;

	getTo("s ", stream);
	m_texCoods.num = lineNo - m_texCoods.line;

	getTo("f ", stream);
	m_indices.line = lineNo;

	// fin del archivo
	getTo("", stream);
	m_indices.num = lineNo - m_indices.line;
}


void MeshLoader::readVertices(std::ifstream& stream)
{
	for(int i = 0; i < m_vertices.num; i++)
	{
		std::string v;
		stream >> v;

		// Leer sus componentes
		double vX, vY, vZ;
		stream >> vX >> vY >> vZ;

		// Añadirlo a la lista
		mesh->vertices[i] = { vX * scale, vY * scale, vZ * scale };
		lineNo++;
	}
}

// Idéntico al método anterior
void MeshLoader::readNormals(std::ifstream& stream)
{
	for(int i = 0; i < m_normales.num; i++)
	{
		std::string vn;
		stream >> vn;

		// Leer sus componentes
		double nX, nY, nZ;
		stream >> nX >> nY >> nZ;

		// Añadirlo a la lista de normales
		normales.push_back({ nX, nY, nZ });
		lineNo++;
	}
}


void MeshLoader::readTexCoords(std::ifstream& stream)
{
	for(int i = 0; i < m_texCoods.num; i++)
	{
		std::string vt;
		stream >> vt;

		// Leer sus componentes
		double tX, tY;
		stream >> tX >> tY;

		// Añadirlo a la lista de coordenadas
		texCoords.push_back({ tX, tY });
		lineNo++;
	}

}

void MeshLoader::readFaces(std::ifstream& stream)
{
	// Leer todos los triángulos
	for(int i = 0; i < m_indices.num; i++)
	{
		std::string f;
		stream >> f;

		glm::ivec3 tri;
		int vt, vn;

		// Cada uno de los 3 vértices del triángulo
		for(int i = 0; i < 3; i++)
		{
			// Primer vértice
			stream >> tri[i];
			stream.get();
			stream >> vt;
			stream.get();
			stream >> vn;
			stream.get();

			// Los índices vienen dados desde el 1
			tri[i]--; vt--; vn--;

			mesh->texCoords[tri[i]] = texCoords[vt];
			mesh->normales[tri[i]] = normales[vn];
		}

		// Añadir el triángulo
		indices.push_back(tri);
	}
}

void MeshLoader::getTo(const std::string& pattern, std::ifstream& stream)
{
	bool stopRead = false;
	std::string texto;
	while (!stream.eof() && !stopRead)
	{
		// 'Marcador' para poder volver a esa posición si nos pasamos
		std::streampos inicio = stream.tellg();
		// Leemos la línea completa
		std::getline(stream, texto);
		if (texto == pattern || texto[0] == pattern[0] && texto[1] == pattern[1])
		{
			//hemos llegado -> volver al marcador
			stopRead = true;
			stream.seekg(inicio);
		}
		else
			lineNo++;
	}
}
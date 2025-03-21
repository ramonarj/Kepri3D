#include "MeshLoader.h"

#include "Mesh.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include "Utils.h"

using namespace glm;


void MeshLoader::loadOBJ(const std::string& filename, float scale)
{
	this->scale = scale;

	// Abrir el archivo (lectura porque solo vamos a leer)
	std::ifstream stream(filename.c_str(), std::ios::in);
	if (!stream.is_open())
	{ throw std::ios_base::failure("MeshLoader ERROR: Could not open file"); }

	// 1) Vuelta de reconocimiento para ver en qué línea empieza cada array de datos
	std::string objString = FileToString(filename.c_str());
	Reconocimiento(objString);

	// 2) Crear la malla y los arrays con el tamaño correspondiente
	mesh = new IndexMesh();
	mesh->type = GL_TRIANGLES;
	mesh->numVertices = m_vertices.num;

	// Volver a la primera línea del flujo
	stream.seekg(0, std::ios::beg);
	lineNo = 1;

	// Leer los atributos en orden
	for(int i = 0; i < 4; i++)
	{
		/* a) Leer los vértices */
		if(dataOrder[i] == 0)
		{
			getTo("v ", stream);
			mesh->vertices = new glm::dvec3[mesh->numVertices];
			readVertices(stream);
		}
		/* b) Leer las normales */
		else if(dataOrder[i] == 1)
		{
			getTo("vn ", stream);
			mesh->normales = new glm::dvec3[mesh->numVertices];
			readNormals(stream);
		}
		/* c) Leer las coordenadas de textura */
		else if(dataOrder[i] == 2)
		{
			getTo("vt ", stream);
			mesh->texCoords = new glm::dvec2[mesh->numVertices];
			readTexCoords(stream);
		}
		/* d) Leer los triángulos */
		else if(dataOrder[i] == 3)
		{
			// Leer cada una de las submallas
			for(int i = 0; i < m_indices.size(); i++)
			{
				getTo("f ", stream);
				readFaces(stream, m_indices[i].num);
			}
			// Rellenar los índices de la malla
			mesh->numIndices = indices.size() * 3;
			mesh->indices = new GLuint[mesh->numIndices];
			for (int i = 0; i < mesh->numIndices; i += 3)
			{
				glm::ivec3 tri = indices[(i / 3) + (i % 3)];

				mesh->indices[i] = tri.x;
				mesh->indices[i + 1] = tri.y;
				mesh->indices[i + 2] = tri.z;
			}
		}
	}

	// No sería necesario
	mesh->SetNormals();

	// Atributos de las submallas
	mesh->m_numSubmallas = m_indices.size();
	mesh->m_divisores = new GLint[mesh->m_numSubmallas + 1];
	mesh->m_divisores[0] = 0;
	int accum = 0;
	for(int i = 1; i <= mesh->m_numSubmallas; i++)
	{
		mesh->m_divisores[i] = accum + m_indices[i - 1].num * 3; // * 3??
		accum += m_indices[i - 1].num * 3;
	}

	// Cerrar el flujo
	stream.close();
}


void MeshLoader::Reconocimiento(const std::string& objString)
{
	// Nos colocamos en la primera línea y vamos avanzando, guardando
	// cuántos vértices, normales, texCoords, etc. hay, y en qué línea empieza cada lista
	lineNo = 1;
	unsigned int dataIndex = 0;
	int submesh = -1;

	std::stringstream ssObj(objString);

	// Leer línea a línea
	unsigned int firstLine = lineNo;
	unsigned int* numElems = nullptr;
	std::string currentPattern = " ";
	std::string s;
	while (std::getline(ssObj, s, '\n'))
	{
		// Buscar directivas separadas por espacios
		std::stringstream patternCheck(s);
		std::string patron;
		std::getline(patternCheck, patron, ' ');
		// a) Sigue siendo la misma directiva
		if (patron == currentPattern) { lineNo++; continue; }
		// b) Ha cambiado
		currentPattern = patron;
		// -> ¿Cuántos elementos tenía la propiedad anterior?
		if (numElems != nullptr)
		{
			*numElems = lineNo - firstLine;
			numElems = nullptr;
		}

		// -> Ver qué tipo de dato es el nuevo
		if (patron == "v")
		{
			m_vertices.line = lineNo;
			numElems = &m_vertices.num;
			dataOrder[dataIndex++] = 0;
		}
		else if (patron == "vn")
		{
			m_normales.line = lineNo;
			numElems = &m_normales.num;
			dataOrder[dataIndex++] = 1;
		}
		else if (patron == "vt")
		{
			m_texCoods.line = lineNo;
			numElems = &m_texCoods.num;
			dataOrder[dataIndex++] = 2;
		}
		else if (patron == "f")
		{
			m_indices.push_back(FaceData(lineNo, 0));
			if (submesh < 0) { submesh++; } // El archivo no tiene etiquetas 'g'
			numElems = &m_indices[submesh].num;
			if(dataIndex < 4)
				dataOrder[dataIndex++] = 3;
		}
		else if(patron == "g")
		{
			submesh++;
		}
		else if(patron == "usemtl")
		{
			std::cout << "use material" << std::endl;
		}

		firstLine = lineNo;
		lineNo++;
	}

	// Por si se acaba el archivo 
	if (numElems != nullptr)
	{
		*numElems = lineNo - firstLine;
	}
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

void MeshLoader::readFaces(std::ifstream& stream, int numTris)
{
	// Leer todos los triángulos de ese grupo de vértices
	for(int i = 0; i < numTris; i++)
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
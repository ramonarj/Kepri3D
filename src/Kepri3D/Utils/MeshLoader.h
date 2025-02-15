#ifndef __MESH_LOADER__
#define __MESH_LOADER__

#include <glm.hpp>
#include <string>
#include <vector>

class IndexMesh;

// Se encarga de cargar una malla de archivo OBJ
class MeshLoader
{
private:
	struct MeshData
	{
		// En qué línea empieza ese dato
		unsigned int line;
		// Cuántos hay de ese dato
		unsigned int num;
	};
public:
	MeshLoader() : mesh(nullptr) {}

	/* Carga la información de un archivo Wavefront (.obj) y rellena la malla con ella */
	void loadOBJ(const std::string& fileName, float scale);

	// Devuelve la malla ya creada
	inline IndexMesh* getMesh() { return mesh; }
private:
	// Malla a devolver
	IndexMesh* mesh;
	float scale;

	// Número de línea por el que vamos leyendo
	unsigned int lineNo;

	// Contenidos del fichero
	std::vector<glm::dvec3> normales;
	std::vector<glm::dvec2> texCoords;
	std::vector<glm::ivec3> indices;

	// Información de la primera lectura
	MeshData m_vertices, m_texCoods, m_normales, m_indices;


	// Métodos auxiliares
	/* Métodos para leer cada atributo */ 
	void readVertices(std::ifstream& stream);
	void readNormals(std::ifstream& stream);
	void readTexCoords(std::ifstream& stream);
	void readFaces(std::ifstream& stream);

	/* Hace una primera lectura del archivo para saber cuánto hay de cada cosa */
	void Reconocimiento(std::ifstream& stream);

	/* Llega al patrón indicado y deja el marcador justo encima */
	void getTo(const std::string& pattern, std::ifstream& stream);


};

#endif
#ifndef __MESH_LOADER__
#define __MESH_LOADER__

#include <glm.hpp>
#include <string>
#include <vector>

class IndexMesh;

// Se encarga de cargar una malla de archivo OBJ
class MeshLoader
{
public:
	MeshLoader() : mesh(nullptr) {}

	/* Carga la información de un archivo Wavefront (.obj) y rellena la malla con ella */
	void loadOBJ(const std::string& fileName);

	// Devuelve la malla ya creada
	inline IndexMesh* getMesh() { return mesh; }
private:
	std::vector<glm::dvec3> vertices;
	std::vector<glm::dvec3> normales;
	std::vector<glm::dvec2> texCoords;
	std::vector<glm::ivec3> indices;

	void readVertices(std::ifstream& stream);
	void readNormals(std::ifstream& stream);
	void readTexCoords(std::ifstream& stream);
	void readFaces(std::ifstream& stream);


	IndexMesh* mesh;
};

#endif
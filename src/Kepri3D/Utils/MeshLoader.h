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
	struct VertexData
	{
		VertexData(){}
		VertexData(unsigned int line, unsigned int num)
		{
			this->line = line;
			this->num = num;
		}
		// En qu� l�nea empieza ese dato
		unsigned int line;
		// Cu�ntos hay de ese dato
		unsigned int num;
	};
	struct FaceData : public VertexData
	{
		FaceData() {}
		FaceData(unsigned int line, unsigned int num) : VertexData(line, num)
		{
			this->material = "";
		}
		// Material usado
		std::string material;
	};
public:
	MeshLoader() : mesh(nullptr) {}

	/* Carga la informaci�n de un archivo Wavefront (.obj) y rellena la malla con ella */
	void loadOBJ(const std::string& fileName, float scale);

	// Devuelve la malla ya creada
	inline IndexMesh* getMesh() { return mesh; }
private:
	// Malla a devolver
	IndexMesh* mesh;
	float scale;

	// N�mero de l�nea por el que vamos leyendo
	unsigned int lineNo;

	// Contenidos del fichero
	std::vector<glm::dvec3> normales;
	std::vector<glm::dvec2> texCoords;
	std::vector<glm::ivec3> indices;

	// Informaci�n de la primera lectura
	VertexData m_vertices, m_texCoods, m_normales;
	// Todos los grupos de tri�ngulos (submeshes)
	std::vector<FaceData> m_indices;
	int dataOrder[4];


	// M�todos auxiliares
	/* M�todos para leer cada atributo */ 
	void readVertices(std::ifstream& stream);
	void readNormals(std::ifstream& stream);
	void readTexCoords(std::ifstream& stream);
	void readFaces(std::ifstream& stream, int numTris);

	/* Hace una primera lectura del archivo para saber cu�nto hay de cada cosa */
	void Reconocimiento(const std::string& objString);

	/* Llega al patr�n indicado y deja el marcador justo encima */
	void getTo(const std::string& pattern, std::ifstream& stream);
};

#endif
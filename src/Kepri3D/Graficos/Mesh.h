#ifndef __MESH__
#define __MESH__

#include <glm.hpp>

#include <string>
#include <vector>
#include "checkML.h"
#include "Defines.h"
#include <glew.h>

class Mesh
{
public:
	/* Constructora */
	Mesh();

	/* Destructora */
	virtual ~Mesh();

	/* Dibuja la malla mandando la información desde los arrays correspondientes */
	virtual void draw();
	/* Dibuja la malla tantas veces como se indique, con una sola llamada a la GPU */
	virtual void drawInstanced(GLuint numInstances);

	// Getters (TEMPORAL)
	inline const void* getVertices() { return vertices; }
	inline glm::dvec4* getColors() { return colores; }
	inline const void* getNormals() { return normales; }
	unsigned int getVerticesNum() const { return numVertices; }
	GLuint getType() const { return type; }
	inline const glm::dvec3 getVolume() { return volume; }

	// Calcula la Bounding Box de la malla
	void calculateVolume();

	// Escala la malla
	void scale(const glm::dvec3& scale);

	/* Creación de mallas de los distintos objetos */
	// Ejes RGB
	static Mesh* generateAxesRGB(GLdouble l);

	// Polígono regular de n lados, dibujado con líneas (LINE_LOOP)
	static Mesh* generatePolygon(GLint sides, GLdouble size);

	// Polígono regular de n lados, dibujado con triángulos rellenos (TRIANGLE_FAN)
	static Mesh* generateFilledPolygon(GLint sides, GLdouble size);

	// Rectángulo usado para botones y RenderTargets (no tiene normales)
	static Mesh* generateRectangle(GLdouble width, GLdouble height);

	// Letra mayúscula hecha con GL_LINE_STRIP de un color dado
	static Mesh* generateLetter(char letter, const glm::vec4& color = {1, 1, 1, 1});

	// Onda hecha con GL_LINE_STRIP de un color dado
	static Mesh* generateWaveform(const std::vector<float>& samples, const glm::vec4& color = { 0, 0.5, 0.1, 1 });

	glm::dvec3 volumeCenter = { 0, 0, 0 };

#ifdef __DEBUG_INFO__
	static GLuint numVerts;
	static GLuint numTris;
#endif

protected:
	/* Número total de vértices */
	int numVertices;

	/* La primitiva que se usará para pintar la malla */
	GLuint type;

	/* Array de vértices */
	glm::dvec3* vertices;

	/* Array de colores para cada uno de los vértices */
	glm::dvec4* colores;

	/* Array de coordenadas de textura para cada uno de los vértices */
	glm::dvec2* texCoords;

	/* Array de vectores normales a cada uno de los vértices */
	glm::dvec3* normales;

	/* Volumen tipo AABB de la malla (para el Frustrum culling) */
	glm::dvec3 volume = { 0, 0, 0 };

	// Métodos auxiliares
	/* Activa/desactiva los arrays de vértices, colores y texturas */
	void enableArrays();
	void disableArrays();

	// Crea un cuadrado centrado en el origen de coordenadas y con una de las 6 orientaciones básicas. 
	// Los vértices creados se añaden al array 'vertices', que debe tener espacio creado para al menos 4 elementos.
	enum PolygonOrientation { Positive_X = 0, Negative_X = 1, Positive_Y = 2, Negative_Y = 3, Positive_Z = 4, Negative_Z = 5 };
	static void createSquare(float lado, const glm::dvec3& centro, PolygonOrientation orientacion, glm::dvec3* vertices);
};


// - - - - - - - - - - - - - - 

class IndexMesh : public Mesh
{
public:
	/* Constructora por defecto */
	IndexMesh();
	/* Destructora */
	virtual ~IndexMesh(); 

	/* Pinta la malla pero usando la tabla de índices, y no otras primitivas */
	void draw() override;
	void drawSubmesh(int i);
	void drawInstanced(GLuint numInstances) override;

	void calculateSubmeshes();
	// Getters (TEMPORAL)
	inline const void* getIndices() { return indices; }
	inline unsigned int getIndicesNum() const { return numIndices; }

	inline GLuint numSubmallas() { return m_numSubmallas; }

	// Creación de mallas in-situ
	/* - - - Formas 3D básicas - - - */
	static IndexMesh* generateCube(GLdouble size, bool equalFaces = true);
	static IndexMesh* generateSphere(GLdouble radio, GLuint paralelos, GLuint meridianos);
	static IndexMesh* generateCilindro(GLdouble radio, GLdouble altura, GLuint lados);
	static IndexMesh* generateCone(GLdouble radio, GLdouble altura, GLuint lados);
	static IndexMesh* generateToro(GLdouble radioExterior, GLdouble radioInterior, GLuint anillos, GLuint lineas);

	/* - - - Cuadrículas y terrenos - - - */
	static IndexMesh* generateGrid(GLint filas, GLint columnas, GLdouble tamFila, GLdouble tamColumna);
	static IndexMesh* generateTerrain(const std::string& filename, GLdouble scale, bool rawFile);
	// Usa la primitiva GL_PATCHES
	static IndexMesh* generateTessGrid(GLint filas, GLint columnas, GLdouble tamFila, GLdouble tamColumna);

	/* - - - Cubemaps - - - */
	static IndexMesh* generateCubemap(GLdouble size = 4.0);

	/* Rellena el vector de tangentes usando los triángulos y las UV */
	void setTangents();

protected:
	friend class MeshLoader;

	/* Tabla de índices para formar los triángulos */
	GLuint* indices;

	/* Array de tangentes a cada uno de los vértices, en la dirección 'u' de la textura */
	glm::vec3* tangentes;

	/* Variable auxiliar para ahorrar cálculos (= numTriangulos * 3) */
	GLuint numIndices;

	/* Número de submallas */
	GLuint m_numSubmallas;

	/* Índices para separar las submallas entre sí */
	GLint* m_divisores;

	// Métodos auxiliares
	/* Rellena el vector de normales usando los triángulos indexados */
	void SetNormals();

};

#endif
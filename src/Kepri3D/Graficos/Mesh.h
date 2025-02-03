#ifndef __MESH__
#define __MESH__

#include <glm.hpp>
#include <glew.h>
#include <string>
#include "checkML.h"


class Mesh
{
public:
	/* Constructora */
	Mesh();

	/* Destructora */
	virtual ~Mesh();

	/* Dibuja la malla mandando la informaci�n desde los arrays correspondientes */
	virtual void draw();
	/* Dibuja la malla tantas veces como se indique, con una sola llamada a la GPU */
	virtual void drawInstanced(GLuint numInstances);

	/* Creaci�n de mallas de los distintos objetos */
	// Ejes RGB
	static Mesh* generateAxesRGB(GLdouble l);

	// Pol�gono regular de n lados, dibujado con l�neas (LINE_LOOP)
	static Mesh* generatePolygon(GLint sides, GLdouble size);

	// Pol�gono regular de n lados, dibujado con tri�ngulos rellenos (TRIANGLE_FAN)
	static Mesh* generateFilledPolygon(GLint sides, GLdouble size);

	// Rect�ngulo usado para botones y RenderTargets (no tiene normales)
	static Mesh* generateRectangle(GLdouble width, GLdouble height);

protected:
	/* N�mero total de v�rtices */
	int numVertices;

	/* La primitiva que se usar� para pintar la malla */
	GLuint type;

	/* Array de v�rtices */
	glm::dvec3* vertices;

	/* Array de colores para cada uno de los v�rtices */
	glm::dvec4* colores;

	/* Array de coordenadas de textura para cada uno de los v�rtices */
	glm::dvec2* texCoords;

	/* Array de vectores normales a cada uno de los v�rtices */
	glm::dvec3* normales;

	// M�todos auxiliares
	/* Activa los arrays de v�rtices, colores y texturas */
	void enableArrays();

	/* Desactiva los arrays de v�rtices, colores y texturas */
	void disableArrays();
};


// - - - - - - - - - - - - - - 

class IndexMesh : public Mesh
{
public:
	/* Constructora por defecto */
	IndexMesh() : indices(nullptr), numIndices(0) {}
	/* Destructora */
	virtual ~IndexMesh() { delete[] indices; }

	/* Pinta la malla pero usando la tabla de �ndices, y no otras primitivas */
	void draw() override;
	void drawInstanced(GLuint numInstances) override;

	// Creaci�n de mallas con tri�ngulos indexados

	/* - - Cubo - - */
	static IndexMesh* generateCube(GLdouble size, bool equalFaces = true);

	/* - - Esfera - - */
	static IndexMesh* generateSphere(GLdouble size, GLuint subdivisions, bool textured = false);

	/* - - Toro - - */
	static IndexMesh* generateToro(GLdouble radioExterior, GLdouble radioInterior, GLuint anillos, GLuint lineas);

	/* - - Grid - - */
	static IndexMesh* generateGrid(GLint filas, GLint columnas, GLdouble tamFila, GLdouble tamColumna);

	/* - - Terreno - - */
	static IndexMesh* generateTerrain(const std::string& filename, GLdouble scale, bool rawFile);

	/* - - Cubemap - - */
	static IndexMesh* generateCubemap(GLdouble size = 4.0);

protected:
	friend class MeshLoader;

	/* Tabla de �ndices para formar los tri�ngulos */
	GLuint* indices;

	/* Variable auxiliar para ahorrar c�lculos (= numTriangulos * 3) */
	GLuint numIndices;

	// M�todos auxiliares
	/* Rellena el vector de normales usando los tri�ngulos indexados */
	void SetNormals();
};

// - - - - - - - - - - - - - - 

class TessMesh : public IndexMesh
{
public:
	/* Constructora por defecto */
	TessMesh() {}
	/* Destructora */
	virtual ~TessMesh() { }

	/* Pinta la malla pero usando la tabla de �ndices, y no otras primitivas */
	void draw() override;

	/* - - Grid - - */
	static TessMesh* generateTessGrid(GLint filas, GLint columnas, GLdouble tamFila, GLdouble tamColumna);

private:

};

#endif
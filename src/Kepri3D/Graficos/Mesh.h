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

	/* Dibuja la malla mandando la información desde los arrays correspondientes */
	virtual void draw();
	/* Dibuja la malla tantas veces como se indique, con una sola llamada a la GPU */
	virtual void drawInstanced(GLuint numInstances);

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

	// Métodos auxiliares
	/* Activa los arrays de vértices, colores y texturas */
	void enableArrays();

	/* Desactiva los arrays de vértices, colores y texturas */
	void disableArrays();
};


// - - - - - - - - - - - - - - 

class IndexMesh : public Mesh
{
public:
	/* Constructora por defecto */
	IndexMesh() : indices(nullptr), tangentes(nullptr), numIndices(0) {}
	/* Destructora */
	virtual ~IndexMesh(); 

	/* Pinta la malla pero usando la tabla de índices, y no otras primitivas */
	void draw() override;
	void drawInstanced(GLuint numInstances) override;

	// Creación de mallas con triángulos indexados

	/* - - Cubo - - */
	static IndexMesh* generateCube(GLdouble size, bool equalFaces = true);

	/* - - Esfera - - */
	static IndexMesh* generateSphere(GLdouble radio, GLuint paralelos, GLuint meridianos);

	/* - - Cilindro - - */
	static IndexMesh* generateCilindro(GLdouble radio, GLdouble altura, GLuint lados);

	/* - - Toro - - */
	static IndexMesh* generateToro(GLdouble radioExterior, GLdouble radioInterior, GLuint anillos, GLuint lineas);

	/* - - Grid - - */
	static IndexMesh* generateGrid(GLint filas, GLint columnas, GLdouble tamFila, GLdouble tamColumna);

	/* - - Terreno - - */
	static IndexMesh* generateTerrain(const std::string& filename, GLdouble scale, bool rawFile);

	/* - - Cubemap - - */
	static IndexMesh* generateCubemap(GLdouble size = 4.0);

	/* - - Grid usando primitiva GL_PATCHES - - */
	static IndexMesh* generateTessGrid(GLint filas, GLint columnas, GLdouble tamFila, GLdouble tamColumna);

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

	// Métodos auxiliares
	/* Rellena el vector de normales usando los triángulos indexados */
	void SetNormals();

};

#endif
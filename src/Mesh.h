#ifndef __MESH__
#define __MESH__

#include <glm.hpp>
#include <freeglut.h>

class Mesh
{
public:
	/* Constructora */
	Mesh() : numVertices (0), type(GL_POINTS), vertices(nullptr), colores(nullptr), texCoords(nullptr) {}
	/* Destructora */
	virtual ~Mesh();


	virtual void draw();

	/* Creación de mallas de los distintos objetos */
	// Ejes RGB
	static Mesh* generateAxesRGB(GLdouble l);
	// Polígono regular de n lados, dibujado con triángulos (TRIANGLE_FAN)
	static Mesh* generatePolygon(GLint sides, GLdouble size);
	// Polígono regular de n lados, dibujado con líneas (LINE_LOOP)
	static Mesh* generateFilledPolygon(GLint sides, GLint size);
	// Cubo con caras opacas
	static Mesh* generateCubeSides(GLdouble size);

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
	IndexMesh() : indices (nullptr), numIndices(0) {}
	/* Destructora */
	virtual ~IndexMesh() { delete[] indices; }

	/* Pinta la malla pero usando la tabla de índices, y no otras primitivas */
	void draw() override;

	/* Creación de mallas con triángulos indexados */
	// Cubo cerrado
	static IndexMesh* generateCube(GLdouble size);

private:
	/* Tabla de índices para formar los triángulos */
	GLuint* indices;

	/* Variable auxiliar para ahorrar cálculos (= numTriangulos * 3) */
	GLuint numIndices;
};

#endif
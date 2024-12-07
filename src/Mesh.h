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

	/* Creaci�n de mallas de los distintos objetos */
	// Ejes RGB
	static Mesh* generateAxesRGB(GLdouble l);
	// Pol�gono regular de n lados, dibujado con tri�ngulos (TRIANGLE_FAN)
	static Mesh* generatePolygon(GLint sides, GLdouble size);
	// Pol�gono regular de n lados, dibujado con l�neas (LINE_LOOP)
	static Mesh* generateFilledPolygon(GLint sides, GLint size);
	// Cubo con caras opacas
	static Mesh* generateCubeSides(GLdouble size);

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
	IndexMesh() : indices (nullptr), numIndices(0) {}
	/* Destructora */
	virtual ~IndexMesh() { delete[] indices; }

	/* Pinta la malla pero usando la tabla de �ndices, y no otras primitivas */
	void draw() override;

	/* Creaci�n de mallas con tri�ngulos indexados */
	// Cubo cerrado
	static IndexMesh* generateCube(GLdouble size);

private:
	/* Tabla de �ndices para formar los tri�ngulos */
	GLuint* indices;

	/* Variable auxiliar para ahorrar c�lculos (= numTriangulos * 3) */
	GLuint numIndices;
};

#endif
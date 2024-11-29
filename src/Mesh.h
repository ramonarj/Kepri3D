#ifndef __MESH__
#define __MESH__

#include <glm.hpp>
#include <freeglut.h>

class Mesh
{
public:
	void draw();

	/* Creaci�n de mallas de los distintos objetos */
	// Ejes RGB
	static Mesh* generateAxesRGB(GLdouble l);
	// Pol�gono regular de n lados, dibujado con tri�ngulos (TRIANGLE_FAN)
	static Mesh* generatePolygon(GLint sides);
	// Pol�gono regular de n lados, dibujado con l�neas (LINE_LOOP)
	static Mesh* generateFilledPolygon(GLint sides);
	// Cubo con caras opacas
	static Mesh* generateCube(GLdouble size);

private:
	/* N�mero total de v�rtices */
	int numVertices;

	/* La primitiva que se usar� para pintar la malla */
	GLuint type;

	/* Array de v�rtices */
	glm::dvec3* vertices;

	/* Color de cada uno de los v�rtices */
	glm::dvec4* colores;
};

#endif
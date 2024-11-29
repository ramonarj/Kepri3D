#ifndef __MESH__
#define __MESH__

#include <glm.hpp>
#include <freeglut.h>

class Mesh
{
public:
	void draw();

	/* Creación de mallas de los distintos objetos */
	// Ejes RGB
	static Mesh* generateAxesRGB(GLdouble l);
	// Polígono regular de n lados, dibujado con triángulos (TRIANGLE_FAN)
	static Mesh* generatePolygon(GLint sides);
	// Polígono regular de n lados, dibujado con líneas (LINE_LOOP)
	static Mesh* generateFilledPolygon(GLint sides);
	// Cubo con caras opacas
	static Mesh* generateCube(GLdouble size);

private:
	/* Número total de vértices */
	int numVertices;

	/* La primitiva que se usará para pintar la malla */
	GLuint type;

	/* Array de vértices */
	glm::dvec3* vertices;

	/* Color de cada uno de los vértices */
	glm::dvec4* colores;
};

#endif
#include "Mesh.h"

#include <glut.h>

#include <iostream>

using namespace glm;

#define PI 3.14159265358979323846

void Mesh::draw()
{	
	// 1) Activar las matrices que vamos a usar y pasarlas a la GPU
	// Vértices
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_DOUBLE, 0, vertices);

	//Colores
	glEnableClientState(GL_COLOR_ARRAY);
	// (coordenadas por vértice [2/3/4], tipo de dato, espacio entre cada dato, puntero al array)
	glColorPointer(4, GL_DOUBLE, 0, colores);

	//Texturas
	if (texCoords != nullptr)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_DOUBLE, 0, texCoords);
	}

	// 2) Dibuja las matrices pasadas a la GPU con el tipo de primitivas dado (type)
	glDrawArrays(type, 0, numVertices);
	//glArrayElement() -> esto dibujaría solamente 1 vértice

	// 3) Volver a dejarlo todo como estaba
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

Mesh* Mesh::generateAxesRGB(GLdouble l)
{
	Mesh* m = new Mesh();
	m->type = GL_LINES;
	m->numVertices = 8;

	// COORDENADAS:
	// X positiva hacia la derecha
	// Y positiva hacia arriba
	// Z positiva hacia el fondo

	/* Array de vértices */
	m->vertices = new dvec3[m->numVertices];
	m->vertices[0] = dvec3(0, 0, 0);
	m->vertices[1] = dvec3(l, 0, 0);
	m->vertices[2] = dvec3(0, 0, 0);
	m->vertices[3] = dvec3(0, l, 0);
	m->vertices[4] = dvec3(0, 0, 0);
	m->vertices[5] = dvec3(0, 0, l);

	// Para probar el DEPTH_TEST
	m->vertices[6] = dvec3(-2 * l, 0, l); // muy lejos
	m->vertices[7] = dvec3(2 * l, 0, l);

	/* Colores para cada vértice */
	m->colores = new dvec4[m->numVertices];
	m->colores[0] = dvec4(1, 0, 0, 1); //rojo
	m->colores[1] = dvec4(1, 1, 0, 1); //amarillo
	m->colores[2] = dvec4(0, 1, 0, 1); //verde
	m->colores[3] = dvec4(0, 1, 0, 1);
	m->colores[4] = dvec4(0, 0, 1, 1); //azul
	m->colores[5] = dvec4(0, 0, 1, 1);

	m->colores[6] = dvec4(0, 0, 0, 0); //negro
	m->colores[7] = dvec4(0, 0, 0, 1);

	// devuelve la malla
	return m;
}

Mesh* Mesh::generatePolygon(GLint sides)
{
	Mesh* m = new Mesh();
	m->type = GL_LINE_LOOP;
	m->numVertices = sides;

	/* Array de vértices */
	m->vertices = new dvec3[m->numVertices];
	for(int i = 0; i < m->numVertices; i++)
	{
		double angle = (360.0 / sides / 2) + (360.0 / sides) * i;
		double posX = cos(angle * 2 * PI / 360.0);
		double posY = sin(angle * 2 * PI / 360.0);
		m->vertices[i] = dvec3(posX / 2, posY / 2, 0);
	}

	/* Colores para cada vértice */
	m->colores = new dvec4[m->numVertices];
	for (int i = 0; i < m->numVertices; i++)
	{
		m->colores[i] = dvec4(0.2, 0.1, 0.2, 1);
	}

	/* Coordenadas de textura (suponiendo un cuadrado) */
	m->texCoords = new dvec2[m->numVertices];
	m->texCoords[0] = dvec2(0, 1);
	m->texCoords[1] = dvec2(0, 0);
	m->texCoords[2] = dvec2(1, 1);
	m->texCoords[3] = dvec2(1, 0);

	// devuelve la malla
	return m;
}

Mesh* Mesh::generateFilledPolygon(GLint sides)
{
	Mesh* m = new Mesh();
	m->type = GL_TRIANGLE_FAN;
	m->numVertices = sides + 2;

	/* Array de vértices */
	m->vertices = new dvec3[m->numVertices];
	m->vertices[0] = dvec3(0, 0, 0); // el centro de la rueda
	for (int i = 1; i < m->numVertices; i++)
	{
		double angle = (360.0 / sides / 2) + (360.0 / sides) * (i - 1);
		double posX = cos(angle * 2 * PI / 360.0);
		double posY = sin(angle * 2 * PI / 360.0);
		m->vertices[i] = dvec3(posX / 2, posY / 2, 0);
	}

	/* Colores para cada vértice */
	m->colores = new dvec4[m->numVertices];
	for (int i = 0; i < m->numVertices; i++)
	{
		m->colores[i] = dvec4(0.2, 0.1, 0.2, 1);
	}

	/* Coordenadas de textura (suponiendo un cuadrado) */
	m->texCoords = new dvec2[m->numVertices];
	m->texCoords[0] = dvec2(0.5, 0.5);
	m->texCoords[1] = dvec2(1, 1);
	m->texCoords[2] = dvec2(0, 1);
	m->texCoords[3] = dvec2(0, 0);
	m->texCoords[4] = dvec2(1, 0);
	m->texCoords[5] = dvec2(1, 1);

	// devuelve la malla
	return m;
}

Mesh* Mesh::generateCube(GLdouble size)
{
	//float f;
	//glGetFloatv(GL_FRONT_FACE, &f);
	//std::cout << f << std::endl;
	//std::cout << GL_CW;

	Mesh* m = new Mesh();
	m->type = GL_TRIANGLE_STRIP;
	m->numVertices = 10;

	/* Array de vértices */
	GLdouble halfSize = size / 2.0;
	m->vertices = new dvec3[m->numVertices];
	//
	m->vertices[0] = dvec3(-halfSize, halfSize, halfSize);
	m->vertices[1] = dvec3(-halfSize, -halfSize, halfSize);
	m->vertices[2] = dvec3(halfSize, halfSize, halfSize);
	m->vertices[3] = dvec3(halfSize, -halfSize, halfSize);

	m->vertices[4] = dvec3(halfSize, halfSize, -halfSize);
	m->vertices[5] = dvec3(halfSize, -halfSize, -halfSize);

	m->vertices[6] = dvec3(-halfSize, halfSize, -halfSize);
	m->vertices[7] = dvec3(-halfSize, -halfSize, -halfSize);

	m->vertices[8] = dvec3(-halfSize, halfSize, halfSize);
	m->vertices[9] = dvec3(-halfSize, -halfSize, halfSize);


	/* Colores para cada vértice */
	m->colores = new dvec4[m->numVertices];
	m->colores[0] = dvec4(1.0, 0.0, 0.0, 1.0);
	for (int i = 1; i < m->numVertices; i++)
	{
		m->colores[i] = dvec4(0.2, 0.1, 0.2, 1);
	}

	// devuelve la malla
	return m;
}

#include "Mesh.h"

#include <iostream>

using namespace glm;

#define PI 3.14159265358979323846

Mesh::~Mesh()
{
	if(texCoords != nullptr)
		delete[] texCoords;
	if(colores != nullptr)
		delete[] colores;
	if(vertices != nullptr)
		delete[] vertices;
}

void Mesh::enableArrays()
{
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
}

void Mesh::disableArrays()
{
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Mesh::draw()
{	
	// 1) Activar los arrays que vamos a usar y pasarlos a la GPU
	enableArrays();

	// 2) Dibuja los arrays pasados a la GPU con la primitiva dada (type)
	glDrawArrays(type, 0, numVertices);
	//glArrayElement() -> esto dibujaría solamente 1 vértice

	// 3) Volver a dejarlo todo como estaba
	disableArrays();
}

Mesh* Mesh::generateAxesRGB(GLdouble l)
{
	Mesh* m = new Mesh();
	m->type = GL_LINES;
	m->numVertices = 6;

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

	/* Colores para cada vértice */
	m->colores = new dvec4[m->numVertices];
	m->colores[0] = dvec4(1, 0, 0, 1); //rojo
	m->colores[1] = dvec4(1, 0, 0, 1); 
	m->colores[2] = dvec4(0, 1, 0, 1); //verde
	m->colores[3] = dvec4(0, 1, 0, 1);
	m->colores[4] = dvec4(0, 0, 1, 1); //azul
	m->colores[5] = dvec4(0, 0, 1, 1);

	// devuelve la malla
	return m;
}

Mesh* Mesh::generatePolygon(GLint sides, GLdouble size)
{
	Mesh* m = new Mesh();
	m->type = GL_LINE_LOOP;
	m->numVertices = sides;

	// 360 / numero de lados
	double angleIncr = (2 * PI / sides);
	// 270 - la mitad del ángulo interno para que quede centrado
	double initialAngle = (3 * PI / 2) - angleIncr / 2; 
	/* Array de vértices */
	m->vertices = new dvec3[m->numVertices];
	for(int i = 0; i < m->numVertices; i++)
	{
		// Posición del vértice actual
		double posX = size * cos(initialAngle + angleIncr * i);
		double posY = size * sin(initialAngle + angleIncr * i);
		m->vertices[i] = dvec3(posX, posY, 0);
	}

	/* Colores para cada vértice */
	m->colores = new dvec4[m->numVertices];
	for (int i = 0; i < m->numVertices; i++)
	{
		m->colores[i] = dvec4(0.2, 0.1, 0.2, 1);
	}

	// devuelve la malla
	return m;
}

Mesh* Mesh::generateFilledPolygon(GLint sides, GLint size)
{
	Mesh* m = new Mesh();
	m->type = GL_TRIANGLE_FAN;
	m->numVertices = sides + 2;

	/* Array de vértices */
	m->vertices = new dvec3[m->numVertices];
	m->vertices[0] = dvec3(0, 0, 0); // el centro de la rueda
	for (int i = 1; i < m->numVertices; i++)
	{
		double angle = (PI / sides) + (2 * PI / sides) * (i - 1);
		double posX = size * cos(angle);
		double posY = size * sin(angle);
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

Mesh* Mesh::generateCubeSides(GLdouble size)
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
	// Frente
	m->vertices[0] = dvec3(-halfSize, halfSize, halfSize);
	m->vertices[1] = dvec3(-halfSize, -halfSize, halfSize);
	m->vertices[2] = dvec3(halfSize, halfSize, halfSize);
	m->vertices[3] = dvec3(halfSize, -halfSize, halfSize);

	// Atrás
	m->vertices[4] = dvec3(halfSize, halfSize, -halfSize);
	m->vertices[5] = dvec3(halfSize, -halfSize, -halfSize);
	m->vertices[6] = dvec3(-halfSize, halfSize, -halfSize);
	m->vertices[7] = dvec3(-halfSize, -halfSize, -halfSize);

	// Cerrar el 'loop'
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


// - - - - - - - - - - - - - - - - - - -

void IndexMesh::draw()
{
	// Activa los arrays de vértices, colores, texturas... y el de índices
	Mesh::enableArrays();
	if (indices != nullptr)
	{
		glEnableClientState(GL_INDEX_ARRAY);
		glIndexPointer(GL_UNSIGNED_INT, 0, indices);
	}

	// Dibuja los triángulos definidos por la tabla de índices
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, indices);

	// Dejarlo todo como estaba
	glDisableClientState(GL_INDEX_ARRAY);
	Mesh::disableArrays();
}

IndexMesh* IndexMesh::generateCube(GLdouble size)
{
	IndexMesh* m = new IndexMesh();
	m->type = GL_TRIANGLES; // irrelevante, porque no se usa
	m->numVertices = 8;
	m->numIndices = 36; // 6 caras x 2 triángulos/cara x 3 vértices/triángulo

	/* Array de vértices */
	GLdouble halfSize = size / 2.0;
	m->vertices = new dvec3[m->numVertices];
	// Cara frontal
	m->vertices[0] = dvec3(-halfSize, halfSize, halfSize);
	m->vertices[1] = dvec3(-halfSize, -halfSize, halfSize);
	m->vertices[2] = dvec3(halfSize, -halfSize, halfSize);
	m->vertices[3] = dvec3(halfSize, halfSize, halfSize);
	// Cara trasera
	m->vertices[4] = dvec3(-halfSize, halfSize, -halfSize);
	m->vertices[5] = dvec3(-halfSize, -halfSize, -halfSize);
	m->vertices[6] = dvec3(halfSize, -halfSize, -halfSize);
	m->vertices[7] = dvec3(halfSize, halfSize, -halfSize);

	/* Colores para cada vértice */
	m->colores = new dvec4[m->numVertices];
	m->colores[0] = dvec4(1.0, 0.0, 0.0, 1.0);
	for (int i = 1; i < m->numVertices; i++)
	{
		m->colores[i] = dvec4(0.2, 0.1, 0.2, 1);
	}
	// De prueba
	m->colores[2] = dvec4(0.0, 0.0, 1.0, 1.0);
	m->colores[7] = dvec4(0.0, 1.0, 0.0, 1.0);

	/* Especificar los triángulos */
	m->indices = new GLuint[m->numIndices];
	// Tiene que haber una forma mejor 
	// Frente
	m->indices[0] = 0; 
	m->indices[1] = 1;
	m->indices[2] = 3;
	
	m->indices[3] = 3;
	m->indices[4] = 1;
	m->indices[5] = 2;

	// Derecha
	m->indices[6] = 3;
	m->indices[7] = 2;
	m->indices[8] = 7;

	m->indices[9] = 7;
	m->indices[10] = 2;
	m->indices[11] = 6;

	// Izquierda
	m->indices[12] = 4;
	m->indices[13] = 5;
	m->indices[14] = 0;

	m->indices[15] = 0;
	m->indices[16] = 5;
	m->indices[17] = 1;

	// Arriba
	m->indices[18] = 4;
	m->indices[19] = 0;
	m->indices[20] = 7;

	m->indices[21] = 7;
	m->indices[22] = 0;
	m->indices[23] = 3;

	// Abajo
	m->indices[24] = 1;
	m->indices[25] = 5;
	m->indices[26] = 2;

	m->indices[27] = 2;
	m->indices[28] = 5;
	m->indices[29] = 6;

	// Atrás
	m->indices[30] = 7;
	m->indices[31] = 6;
	m->indices[32] = 4;

	m->indices[33] = 4;
	m->indices[34] = 6;
	m->indices[35] = 5;

	return m;
}
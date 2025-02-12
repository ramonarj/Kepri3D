#include "Mesh.h"

#include <iostream>
#include <fstream>
#include "Utils.h"
#include "Pixmap32RGBA.h"

using namespace glm;

#ifdef __DEBUG_INFO__
GLuint Mesh::numVerts = 0;
GLuint Mesh::numTris = 0;
#endif

Mesh::Mesh() : numVertices(0), type(GL_POINTS), vertices(nullptr), colores(nullptr), 
	texCoords(nullptr), normales(nullptr)
{

}

Mesh::~Mesh()
{
	if(texCoords != nullptr)
		delete[] texCoords;
	if(colores != nullptr)
		delete[] colores;
	if(vertices != nullptr)
		delete[] vertices;
	if (normales != nullptr)
		delete[] normales;
}

void Mesh::enableArrays()
{
	// Vértices
	if(vertices != nullptr)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_DOUBLE, 0, vertices);
		// Para el vertex shader
		glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, vertices);
		glEnableVertexAttribArray(0);
	}

	// Colores
	if (colores != nullptr)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		// (coordenadas por vértice [2/3/4], tipo de dato, espacio entre cada dato, puntero al array)
		glColorPointer(4, GL_DOUBLE, 0, colores);

		// Para el vertex shader
		glVertexAttribPointer(1, 4, GL_DOUBLE, GL_FALSE, 0, colores);
		glEnableVertexAttribArray(1);
	}

	// Coordenadas de textura
	if (texCoords != nullptr)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_DOUBLE, 0, texCoords);

		// Para el vertex shader
		glVertexAttribPointer(2, 2, GL_DOUBLE, GL_FALSE, 0, texCoords);
		glEnableVertexAttribArray(2);
	}

	// Normales 
	if(normales != nullptr)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_DOUBLE, 0, normales);

		// Para el vertex shader
		glVertexAttribPointer(3, 3, GL_DOUBLE, GL_FALSE, 0, normales);
		glEnableVertexAttribArray(3);
	}
}

void Mesh::disableArrays()
{
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
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
#ifdef __DEBUG_INFO__
	numVerts += numVertices;
#endif
}

void Mesh::drawInstanced(GLuint numInstances)
{
	// 1) Activar los arrays que vamos a usar y pasarlos a la GPU
	enableArrays();

	// 2) Dibuja los arrays pasados a la GPU con la primitiva dada (type)
	glDrawArraysInstanced(type, 0, numVertices, numInstances);

	// 3) Volver a dejarlo todo como estaba
	disableArrays();
#ifdef __DEBUG_INFO__
	numVerts += numVertices * numInstances;
#endif
}

// - - - - - - - - - - - - - - - - - - -

IndexMesh::~IndexMesh() 
{
	if (indices != nullptr)
		delete[] indices;
	if (tangentes != nullptr)
		delete[] tangentes;
}

void IndexMesh::draw()
{
	// Activa los arrays de vértices, colores, texturas...
	Mesh::enableArrays();
	// Tangentes
	if (tangentes != nullptr)
	{
		// Para el vertex shader
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, tangentes);
		glEnableVertexAttribArray(4);
	}

	// Dibuja los triángulos definidos por la tabla de índices
	glDrawElements(type, numIndices, GL_UNSIGNED_INT, indices);

	// Dejarlo todo como estaba
	glDisableVertexAttribArray(4);
	Mesh::disableArrays();

#ifdef __DEBUG_INFO__
	numVerts += numVertices;
	numTris += numIndices / 3.0;
#endif
}

void IndexMesh::drawInstanced(GLuint numInstances)
{
	// 1) Activar los arrays que vamos a usar y pasarlos a la GPU
	enableArrays();

	// 2) Dibuja los arrays pasados a la GPU con la primitiva dada (type)
	glDrawElementsInstanced(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, indices, numInstances);

	// 3) Volver a dejarlo todo como estaba
	disableArrays();
#ifdef __DEBUG_INFO__
	numVerts += numVertices * numInstances;
	numTris += numIndices / 3.0;
#endif
}

void IndexMesh::SetNormals()
{
	if (normales == nullptr)
		normales = new dvec3[numVertices];

	// Se inicializan todas a 0
	for (int i = 0; i < numVertices; i++)
		normales[i] = { 0,0,0 };

	// Recorrido de los triángulos
	for (int i = 0; i < numIndices; i += 3)
	{
		// Los 3 vértices del triángulo
		glm::dvec3 a = vertices[indices[i]];
		glm::dvec3 b = vertices[indices[i + 1]];
		glm::dvec3 c = vertices[indices[i + 2]];

		// Vector normal al triángulo
		dvec3 n = glm::normalize(glm::cross(c - b, a - b));

		// Sumárselo a sus vértices
		normales[indices[i]] += n;
		normales[indices[i + 1]] += n;
		normales[indices[i + 2]] += n;
	}

	// Normalizar las normales
	for (int i = 0; i < numVertices; i++)
		normales[i] = glm::normalize(normales[i]);
}

// Muy similar a setNormals()
void IndexMesh::setTangents()
{
	tangentes = new glm::vec3[numVertices];
	// Se inicializan todas a 0
	for (int i = 0; i < numVertices; i++)
		tangentes[i] = { 0,0,0 };

	// Para cada triángulo:
	for (int i = 0; i < numIndices; i+=3)
	{
		// Los 3 vértices del triángulo (y sus UVs)
		glm::dvec3 a = vertices[indices[i]];
		glm::dvec3 b = vertices[indices[i + 1]];
		glm::dvec3 c = vertices[indices[i + 2]];

		glm::dvec2 UVa = texCoords[indices[i]];
		glm::dvec2 UVb = texCoords[indices[i + 1]];
		glm::dvec2 UVc = texCoords[indices[i + 2]];

		// Incremento en la posición y las UV
		glm::vec3 edge1 = b - a;
		glm::vec3 edge2 = c - a;
		glm::vec2 deltaUV1 = UVb - UVa;
		glm::vec2 deltaUV2 = UVc - UVa;

		// Calcular la tangente al triángulo
		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		glm::dvec3 t = { f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x) ,
			f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
			f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z) };

		// Sumársela a sus vértices
		tangentes[indices[i]] += t;
		tangentes[indices[i + 1]] += t;
		tangentes[indices[i + 2]] += t;
	}
	// Normalizar las tangentes
	for (int i = 0; i < numVertices; i++)
		tangentes[i] = glm::normalize(tangentes[i]);
}

// - - - - - - - - - Mallas - - - - - - - - - - //

Mesh* Mesh::generateAxesRGB(GLdouble l)
{
	Mesh* m = new Mesh();
	m->type = GL_LINES;
	m->numVertices = 6;

	/* Array de vértices */
	m->vertices = new dvec3[m->numVertices]
	{
		{0, 0, 0}, {l, 0, 0},
		{0, 0, 0}, {0, l, 0},
		{0, 0, 0}, {0, 0, l}
	};

	/* Colores para cada vértice */
	m->colores = new dvec4[m->numVertices]
	{
		{1, 0, 0, 1}, {1, 0, 0, 1}, //rojo
		{0, 1, 0, 1}, {0, 1, 0, 1}, //verde
		{0, 0, 1, 1}, {0, 0, 1, 1} //azul
	};

	// Devuelve la malla
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
		m->colores[i] = dvec4(0.8, 0.8, 0.8, 1);

	return m;
}

Mesh* Mesh::generateFilledPolygon(GLint sides, GLdouble size)
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
		m->colores[i] = dvec4(0.8, 0.8, 0, 1);

	/* Coordenadas de textura (suponiendo un cuadrado) */
	m->texCoords = new dvec2[m->numVertices]{
		{0.5, 0.5}, {1, 1}, {0, 1}, {0, 0}, {1, 0}, {1, 1}
	};

	return m;
}

Mesh* Mesh::generateRectangle(GLdouble width, GLdouble height)
{
	Mesh* m = new Mesh();
	m->type = GL_TRIANGLE_STRIP;
	m->numVertices = 4;

	/* Array de vértices */
	m->vertices = new dvec3[m->numVertices]
	{
		{-width / 2.0, height / 2.0, 0},
		{-width / 2.0, -height / 2.0, 0},
		{width / 2.0, height / 2.0, 0},
		{width / 2.0, -height / 2.0, 0}
	};

	/* Coordenadas de textura*/
	m->texCoords = new dvec2[m->numVertices] { {0,1}, {0, 0}, {1, 1}, {1, 0} };

	/* Colores*/
	m->colores = new dvec4[m->numVertices];
	for (int i = 0; i < m->numVertices; i++)
		m->colores[i] = { 0.8,0.8, 0.8, 1.0 };

	return m;
}

Mesh* Mesh::generateLetter(char l, const glm::vec4& color)
{
	Mesh* m = new Mesh();
	m->type = GL_LINE_STRIP;
	switch(l)
	{
	case 'A':
		m->numVertices = 6;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 0, 0}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0}, {1, 0.5, 0}, {0, 0.5, 0} };
		break;
	case 'B':
		m->numVertices = 7;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 0, 0}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0}, {0, 0, 0}, {0, 0.5, 0}, {1, 0.5, 0} };
		break;
	case 'C':
		m->numVertices = 4;
		m->vertices = new dvec3[m->numVertices]
		{ {1, 1, 0}, {0, 1, 0}, {0, 0, 0},{1, 0, 0} };
		break;
	case 'D':
		m->numVertices = 5;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 0, 0}, {0, 1, 0}, {1, 0.8, 0},{1, 0.2, 0}, {0, 0, 0} };
		break;
	case 'E':
		m->numVertices = 7;
		m->vertices = new dvec3[m->numVertices]
		{ {1, 1, 0}, {0, 1, 0}, {0, 0, 0}, {1, 0, 0} , {0, 0, 0} , {0, 0.5, 0}, {1, 0.5, 0} };
		break;
	case 'F':
		m->numVertices = 6;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 0, 0}, {0, 1, 0}, {1, 1, 0}, {0, 1, 0} , {0, 0.5, 0} , {1, 0.5, 0} };
		break;
	case 'G':
		m->numVertices = 6;
		m->vertices = new dvec3[m->numVertices]
		{ {1, 1, 0} , {0, 1, 0}, {0, 0, 0}, {1, 0, 0}, {1, 0.5, 0} , {0.5, 0.5, 0} };
		break;
	case 'H':
		m->numVertices = 6;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 1, 0} , {0, 0, 0}, {0, 0.5, 0}, {1, 0.5, 0}, {1, 1, 0} , {1, 0, 0} };
		break;
	case 'I':
		m->numVertices = 6;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 1, 0}, {1, 1, 0}, {0.5, 1, 0}, {0.5, 0, 0} , {0, 0, 0} , {1, 0, 0} };
		break;
	case 'J':
		m->numVertices = 5;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 1, 0}, {1, 1, 0}, {0.5, 1, 0}, {0.5, 0, 0} , {0, 0, 0} };
		break;
	case 'K':
		m->numVertices = 6;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 1, 0}, {0, 0, 0}, {0, 0.5, 0}, {1, 1, 0} , {0, 0.5, 0}, {1, 0, 0} };
		break;
	case 'L':
		m->numVertices = 3;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 1, 0}, {0, 0, 0}, {1, 0, 0} };
		break;
	case 'M':
		m->numVertices = 5;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 0, 0}, {0, 1, 0}, {0.5, 0.5, 0}, {1, 1, 0}, {1, 0, 0} };
		break;
	case 'N':
		m->numVertices = 4;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 0, 0}, {0, 1, 0}, {1, 0, 0}, {1, 1, 0} };
		break;
	case 'O':
		m->numVertices = 5;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 1, 0}, {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0} };
		break;
	case 'P':
		m->numVertices = 5;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 0, 0}, {0, 1, 0}, {1, 1, 0}, {1, 0.5, 0}, {0, 0.5, 0} };
		break;
	case 'Q':
		m->numVertices = 7;
		m->vertices = new dvec3[m->numVertices]
		{ {0.8, 0.2, 0}, {0.8, 1, 0}, {0, 1, 0}, {0, 0.2, 0}, {0.8, 0.2, 0}, {0.5, 0.5, 0}, {1, 0, 0} };
		break;
	case 'R':
		m->numVertices = 6;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 0, 0}, {0, 1, 0}, {1, 1, 0}, {1, 0.5, 0}, {0, 0.5, 0} , {1, 0, 0} };
		break;
	case 'S':
		m->numVertices = 6;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 0, 0}, {1, 0, 0}, {1, 0.5, 0}, {0, 0.5, 0}, {0, 1, 0}, {1, 1, 0} };
		break;
	case 'T':
		m->numVertices = 4;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 1, 0}, {1, 1, 0}, {0.5, 1, 0}, {0.5, 0, 0} };
		break;
	case 'U': 
		m->numVertices = 4;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 1, 0}, {0, 0, 0}, {1, 0, 0}, {1, 1, 0 } };
		break;
	case 'V':
		m->numVertices = 3;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 1, 0}, {0.5, 0, 0}, {1, 1, 0} };
		break;
	case 'W':
		m->numVertices = 5;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 1, 0}, {0.25, 0, 0}, {0.5, 0.5, 0}, {0.75, 0, 0}, {1, 1, 0} };
		break;
	case 'X':
		m->numVertices = 5;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 0, 0}, {1, 1, 0}, {0.5, 0.5, 0}, {0, 1, 0}, {1, 0, 0} };
		break;
	case 'Y':
		m->numVertices = 6;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 1, 0}, {0, 0.5, 0}, {1, 0.5, 0}, {1, 1, 0}, {1, 0, 0}, {0, 0, 0 } };
		break;
	case 'Z':
		m->numVertices = 4;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 1, 0}, {1, 1, 0}, {0, 0, 0}, {1, 0, 0} };
		break;
	case '0':
		m->numVertices = 6;
		m->vertices = new dvec3[m->numVertices]
		{ {1, 1, 0}, {0, 1, 0}, {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 0, 0} };
		break;
	case '1':
		m->numVertices = 3;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 0.5, 0}, {1, 1, 0}, {1, 0, 0} };
		break;
	case '2':
		m->numVertices = 6;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 1, 0}, {1, 1, 0}, {1, 0.5, 0} , {0, 0.5, 0}, {0, 0, 0}, {1, 0, 0} };
		break;
	case '3':
		m->numVertices = 7;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 1, 0}, {1, 1, 0}, {1, 0.5, 0}, {0, 0.5, 0}, {1, 0.5, 0}, {1, 0, 0}, {0, 0, 0} };
		break;
	case '4':
		m->numVertices = 5;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 1, 0}, {0, 0.5, 0}, {1, 0.5, 0}, {1, 1, 0}, {1, 0, 0} };
		break;
	case '5':
		m->numVertices = 6;
		m->vertices = new dvec3[m->numVertices]
		{ {1, 1, 0}, {0, 1, 0}, {0, 0.5, 0}, {1, 0.5, 0}, {1, 0, 0} , {0, 0, 0} };
		break;
	case '6':
		m->numVertices = 6;
		m->vertices = new dvec3[m->numVertices]
		{ {1, 1, 0}, {0, 1, 0}, {0, 0, 0}, {1, 0, 0}, {1, 0.5, 0}, {0, 0.5, 0} };
		break;
	case '7':
		m->numVertices = 3;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 1, 0}, {1, 1, 0}, {0, 0, 0} };
		break;
	case '8': // Igual que la B
		m->numVertices = 7;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 0, 0}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0}, {0, 0, 0}, {0, 0.5, 0}, {1, 0.5, 0} };
		break;
	case '9':
		m->numVertices = 5;
		m->vertices = new dvec3[m->numVertices]
		{ {1, 0, 0}, {1, 1, 0}, {0, 1, 0}, {0, 0.5, 0}, {1, 0.5, 0} };
		break;
	case '-':
		m->numVertices = 2;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 0.5, 0}, {1, 0.5, 0} };
		break;
	case '/':
		m->numVertices = 2;
		m->vertices = new dvec3[m->numVertices]
		{ {0, 0, 0}, {1, 1, 0} };
		break;
	default:
		break;
	}

	/* Colores */
	m->colores = new glm::dvec4[m->numVertices];
	for (int i = 0; i < m->numVertices; i++)
		m->colores[i] = color;

	return m;
}


// - - - - - - - - Mallas indexadas - - - - - - - - - //

IndexMesh* IndexMesh::generateCube(GLdouble size, bool equalFaces)
{
	IndexMesh* m = new IndexMesh();
	m->type = GL_TRIANGLES; // irrelevante, porque no se usa
	m->numVertices = 6 * 4; //para poder texturizar cada cara
	m->numIndices = 36; // 6 caras x 2 triángulos/cara x 3 vértices/triángulo

	/* Array de vértices */
	size /= 2.0;
	m->vertices = new dvec3[m->numVertices]
	{
		// Cara frontal
		{-size, size, size},
		{-size, -size, size},
		{size, -size, size},
		{size, size, size},

		// Cara derecha
		{size, size, size},
		{size, -size, size},
		{size, -size, -size},
		{size, size, -size},

		// Cara trasera
		{size, size, -size},
		{size, -size, -size},
		{-size, -size, -size},
		{-size, size, -size},

		// Cara izquierda
		{-size, size, -size},
		{-size, -size, -size},
		{-size, -size, size},
		{-size, size, size},

		// Cara superior
		{-size, size, -size},
		{-size, size, size},
		{size, size, size},
		{size, size, -size},

		// Cara inferior
		{-size, -size, size},
		{-size, -size, -size},
		{size, -size, -size},
		{size, -size, size}
	};

	/* Colores para cada vértice */
	m->colores = new dvec4[m->numVertices];
	for (int i = 0; i < m->numVertices; i++)
		m->colores[i] = dvec4(0.8, 0.8, 0.8, 1);

	/* Especificar los triángulos */
	m->indices = new GLuint[m->numIndices]
	// Para cada cara...
	//for(int i = 0; i < 6; i++)
	//{
	//	// Primer triángulo
	//	m->indices[6 * i] = i;
	//	m->indices[6 * i + 1] = i + 1;
	//	m->indices[6 * i + 2] = i + 3;

	//	// Segundo triángulo
	//	m->indices[6 * i + 3] = i + 3;
	//	m->indices[6 * i + 4] = i + 3;
	//	m->indices[6 * i + 5] = i + 3;
	//}
	{
		// Frente
		0, 1, 3,
		3, 1, 2,

		// Derecha
		4, 5, 7,
		7, 5, 6,

		// Atrás
		8, 9, 11,
		11, 9, 10,

		// Izquierda
		12, 13, 15,
		15, 13, 14,

		// Arriba
		16, 17, 19,
		19, 17, 18,

		// Abajo
		20, 21, 23,
		23, 21, 22,
	};

	/* Coordenadas de textura */
	// Usar la textura completa para cada cara
	if (equalFaces)
	{
		m->texCoords = new dvec2[m->numVertices];
		for (int i = 0; i < 6; i++)
		{
			m->texCoords[i * 4] = { 0, 1 };
			m->texCoords[i * 4 + 1] = { 0, 0 };
			m->texCoords[i * 4 + 2] = { 1, 0 };
			m->texCoords[i * 4 + 3] = { 1, 1 };
		}
	}
	// Usar distintas (6) partes de la textura las distintas caras
	else
	{
		m->texCoords = new dvec2[m->numVertices]
		{
			// Frente
			{0.251, 0.333}, {0.251, 0}, {0.5, 0}, {0.5, 0.333},

			// Derecha
			{0.501, 0.334}, {0.75, 0.334}, {0.75, 0.66}, {0.501, 0.66},

			// Atrás
			{0.5, 0.667}, {0.5, 1}, {0.251, 1}, {0.251, 0.667},

			// Izquierda
			{0.25, 0.666}, {0, 0.666}, {0, 0.334}, {0.25, 0.334},

			// Arriba
			{0.251, 0.666}, {0.251, 0.334}, {0.5, 0.334}, {0.5, 0.666},

			// Abajo
			{1, 0.334}, {1, 0.666}, {0.75, 0.666}, {0.75, 0.334}
		};
	}

	/* Generar las normales */
	m->SetNormals();

	return m;
}

IndexMesh* IndexMesh::generateSphere(GLdouble radio, GLuint paralelos, GLuint meridianos)
{
	IndexMesh* m = new IndexMesh();
	m->type = GL_TRIANGLES; 
	m->numVertices = (meridianos + 1) * paralelos + 2; // meridiano repetido para poder envolver con la textura
	m->numIndices = 6 * meridianos * paralelos;

	/* Array de  vértices */
	m->vertices = new dvec3[m->numVertices];
	GLdouble angleIncr = 2 * PI / meridianos;
	GLdouble angleIncrY = PI / (paralelos + 1);
	GLdouble initialY = PI / 2 - angleIncrY;
	int k = 0;
	// Recorremos por meridianos, añadiendo uno extra
	for (int j = 0; j < meridianos + 1; j++)
	{
		for (int i = 0; i < paralelos; i++)
		{
			GLdouble subradio = radio * cos(initialY - angleIncrY * i);
			GLdouble posX = subradio * cos(angleIncr * j);
			GLdouble posY = radio * sin(initialY - angleIncrY * i);
			GLdouble posZ = subradio * -sin(angleIncr * j);
			m->vertices[k] = { posX,posY,posZ };
			k++;
		}
	}
	// Las 2 tapas
	int centroTapa = m->numVertices - 2;
	int centroBase = m->numVertices - 1;
	m->vertices[centroTapa] = { 0, radio, 0 };
	m->vertices[centroBase] = { 0, -radio, 0 };

	// Índices
	m->indices = new GLuint[m->numIndices];
	k = 0;
	for (int j = 0; j < meridianos; j++)
	{
		// Tapa de arriba
		m->indices[k] = centroTapa;
		m->indices[k + 1] = paralelos * j;
		m->indices[k + 2] = paralelos * (j + 1);
		k += 3;
		// Cuerpo de la esfera
		for (int i = 0; i < paralelos - 1; i++)
		{
			m->indices[k] = paralelos * j + i;
			m->indices[k + 1] = paralelos * j + i + 1;
			m->indices[k + 2] = paralelos * (j + 1) + i;

			m->indices[k + 3] = paralelos * (j + 1) + i;
			m->indices[k + 4] = paralelos * j + i + 1;
			m->indices[k + 5] = paralelos * (j + 1) + i + 1;
			k += 6;
		}
		// Tapa de abajo
		m->indices[k] = paralelos * (j + 1) - 1;
		m->indices[k + 1] = centroBase;
		m->indices[k + 2] = paralelos * (j + 2) - 1;
		k += 3;
	}

	/* Coordenadas de textura */
	m->texCoords = new dvec2[m->numVertices];
	for (int j = 0; j < meridianos + 1; j++)
	{
		GLdouble texX = j * (1.0 / meridianos);
		for (int i = 0; i < paralelos; i++)
		{
			GLdouble texY = 1.0 - 1.0 / (paralelos + 1) - (1.0 / (paralelos + 1) * i);
			m->texCoords[j * paralelos + i] = { texX, texY };
		}
	}
	// Tapas
	m->texCoords[centroTapa] = { 0.5, 1 };
	m->texCoords[centroBase] = { 0.5, 0 };

	/* Colores */
	m->colores = new dvec4[m->numVertices];
	for (int i = 0; i < m->numVertices; i++)
		m->colores[i] = dvec4(0, 1, 0, 1);

	/* Normales */
	m->SetNormals();

	return m;
}

IndexMesh* IndexMesh::generateCilindro(GLdouble radio, GLdouble altura, GLuint lados)
{
	IndexMesh* m = new IndexMesh();
	m->type = GL_TRIANGLES;
	m->numVertices = 4 * lados + 2; // teniendo en cuenta que son bordes duros (vértices repetidos)
	m->numIndices = 4 * lados * 3;

	// Vértices
	float angleIncr = 2 * PI / lados;
	m->vertices = new glm::dvec3[m->numVertices];
	for(int i = 0; i < lados; i++)
	{
		double posX = radio * cos(angleIncr * i);
		double posZ = radio * -sin(angleIncr * i);
		// Base repetida
		m->vertices[i] = { posX, 0, posZ };
		m->vertices[i + lados] = m->vertices[i];
		// Techo repetido
		m->vertices[i + lados * 2] = { posX, altura, posZ };
		m->vertices[i + lados * 3] = m->vertices[i + lados * 2];
	}
	// Los 2 centros de las tapas
	int centroBase = m->numVertices - 2;
	int centroTapa = m->numVertices - 1;
	m->vertices[centroBase] = { 0, 0, 0 };
	m->vertices[centroTapa] = { 0, altura, 0 };


	// Índices
	m->indices = new GLuint[m->numIndices];
	int k = 0;
	for (int i = 0; i < lados * 3; i+=3)
	{
		// Triángulo de la base
		m->indices[i] = k;
		m->indices[i + 1] = centroBase;
		m->indices[i + 2] = (k + 1) % lados;

		// Triángulos del cuerpo
		m->indices[lados * 3 + i * 2] = lados + k;
		m->indices[lados * 3 + i * 2 + 1] = lados + (k + 1) % lados;
		m->indices[lados * 3 + i * 2 + 2] = lados * 2 + k;

		m->indices[lados * 3 + i * 2 + 3] = lados * 2 + k;
		m->indices[lados * 3 + i * 2 + 4] = lados + (k + 1) % lados;
		m->indices[lados * 3 + i * 2 + 5] = lados * 2 + (k + 1) % lados;

		// Triángulo de la tapa
		m->indices[lados * 9 + i] = lados * 3 + k;
		m->indices[lados * 9 + i + 1] = lados * 3 + (k + 1) % lados;
		m->indices[lados * 9 + i + 2] = centroTapa;
		k++;
	}

	// Colores
	m->colores = new glm::dvec4[m->numVertices];
	for (int i = 0; i < m->numVertices; i++)
		m->colores[i] = { 0.8, 0.8, 0.8, 1 };

	// Normales
	m->SetNormals();

	// Coordenadas de textura
	m->texCoords = new glm::dvec2[m->numVertices];
	for (int i = 0; i < lados; i++)
	{
		double coordX = (cos(angleIncr * i) + 1) / 2.0;
		double coordY = (sin(angleIncr * i) + 1) / 2.0;
		// Base
		m->texCoords[i] = { 1 - coordX, coordY };
		// Techo
		m->texCoords[i + lados * 3] = { coordX, coordY };
		// Cuerpo
		m->texCoords[i + lados] = { 1.0 / lados * i, 0 };
		m->texCoords[i + lados * 2] = { 1.0 / lados * i, 1 };
	}
	m->texCoords[centroBase] = m->texCoords[centroTapa] = { 0.5, 0.5 };

	return m;
}

IndexMesh* IndexMesh::generateToro(GLdouble radExt, GLdouble radInt, GLuint anillos, GLuint lineas)
{
	// TODO: poner coordenadas de textura
	IndexMesh* m = new IndexMesh();
	m->type = GL_TRIANGLES;
	GLuint verticesSeccion = (lineas - 2) * 2 + 2;
	m->numVertices = anillos * verticesSeccion;

	/* Array de  vértices */
	m->vertices = new dvec3[m->numVertices];
	// Cortes cenitales
	GLuint count = 0;
	GLdouble angleIncrInt = 2.0 * PI / verticesSeccion;
	GLdouble angleIncrExt = 2.0 * PI / anillos;
	for (int i = 0; i < verticesSeccion; i++)
	{
		GLdouble posY = radInt * sin(PI / 2 - i * angleIncrInt); //hacia fuera
		// Dibujar un polígono de "anillos" lados
		for (int j = 0; j < anillos; j++)
		{
			GLdouble posX = (radExt + radInt * cos(PI / 2 - i * angleIncrInt))
				* cos(3 * PI / 2 + j * angleIncrExt);
			GLdouble posZ = (radExt + radInt * sin(i * angleIncrInt))
				* sin(3 * PI / 2 + j * angleIncrExt);
			m->vertices[count] = dvec3(posX, posY, posZ);
			count++;
		}
	}

	/* Colores */
	m->colores = new dvec4[m->numVertices];
	for (int i = 0; i < m->numVertices; i++)
		m->colores[i] = { 0.8,0.8, 0.8, 1.0 };

	/* Índices */
	// Se da el caso de que nºvertices = nºquads. (indices = quads * 2 * 3)
	m->numIndices = m->numVertices * 6;
	m->indices = new GLuint[m->numIndices];
	count = 0;
	// Iterar las lineas
	for (int i = 0; i < verticesSeccion; i++) 
	{
		// Iterar cada anillo
		for (int j = 0; j < anillos; j++)
		{
			// Los 4 vértices del 'quad' (el 2, 3 y 4 son los que pueden salirse de rango)
			GLuint v1, v2, v3, v4;
			v1 = i * anillos + j;
			v2 = i * anillos + ((j + 1) % anillos);
			v3 = ((i + 1) % verticesSeccion) * anillos + j;
			v4 = ((i + 1) % verticesSeccion) * anillos + ((j + 1) % anillos);

			// Primer triángulo
			m->indices[count] = v1;
			m->indices[count + 1] = v2;
			m->indices[count + 2] = v3;
			// Segundo triángulo
			m->indices[count + 3] = v3;
			m->indices[count + 4] = v2;
			m->indices[count + 5] = v4;

			count+=6;
		}
	}
	m->indices[m->numIndices - 1] = 0;

	/* Normales */
	m->SetNormals();

	return m;
}

IndexMesh* IndexMesh::generateGrid(GLint filas, GLint columnas, GLdouble tamFila, GLdouble tamColumna)
{
	IndexMesh* m = new IndexMesh();
	m->type = GL_TRIANGLES;
	m->numVertices = (filas + 1) * (columnas + 1);
	m->numIndices = filas * columnas * 2 * 3;

	/* Lista de vértices */
	m->vertices = new dvec3[m->numVertices];
	GLuint k = 0; //var. auxiliar
	GLdouble iniZ = -filas / 2.0 * tamFila;
	for(int i = 0; i < filas + 1; i++)
	{
		GLdouble iniX = -columnas / 2.0 * tamColumna;
		for(int j = 0; j < columnas + 1; j++)
		{
			m->vertices[k] = { iniX + tamColumna * j, 0, iniZ + tamFila * i };
			k++;
		}
	}

	/* Lista de colores */
	m->colores = new dvec4[m->numVertices];
	for (int i = 0; i < m->numVertices; i++)
		m->colores[i] = dvec4(0.8, 0.8, 0.8, 1);

	/* Coordenadas de textura -> se podría meter en el mismo bucle que los vértices */
	m->texCoords = new dvec2[m->numVertices];
	GLdouble texIncrX = 1.0 / columnas;
	GLdouble texIncrZ = 1.0 / filas;
	k = 0;
	for (int i = 0; i < filas + 1; i++)
	{
		for (int j = 0; j < columnas + 1; j++)
		{
			m->texCoords[k] = { texIncrX * j , 1 - texIncrZ * i };
			k++;
		}
	}

	/* Lista de triángulos (índices) */
	m->indices = new GLuint[m->numIndices];
	k = 0;
	for(int i = 0; i < filas; i++)
	{
		// Pintar cada cuadrado como 2 triángulos
		for (int j = 0; j < columnas; j++)
		{
			// Primer triángulo
			m->indices[k] = i * (columnas + 1) + j;
			m->indices[k + 1] = (i + 1) * (columnas + 1) + j;
			m->indices[k + 2] = i * (columnas + 1) + j + 1;
			k += 3;

			// Segundo triángulo
			m->indices[k] = i * (columnas + 1) + j + 1;
			m->indices[k + 1] = (i + 1) * (columnas + 1) + j;
			m->indices[k + 2] = (i + 1) * (columnas + 1) + j + 1;
			k += 3;
		}
	}

	/* Normales */
	m->normales = new dvec3[m->numVertices];
	for (int i = 0; i < m->numVertices; i++)
		m->normales[i] = { 0,1, 0 };

	return m;
}

IndexMesh* IndexMesh::generateTerrain(const std::string& filename, GLdouble scale, bool rawFile)
{
	IndexMesh* m;

	/* a) Es un archivo RAW*/
	if(rawFile)
	{
		// Crear la malla y leer los datos del archivo
		m = generateGrid(256, 256, scale, scale);

		// Abrimos el archivo binario
		std::ifstream file(filename, std::ios::binary); // "terrain.raw": 257*257 unsigned chars
		if (!file.is_open())
			return nullptr;

		unsigned char* data = new unsigned char[m->numVertices];
		// char = byte -> leer en data un bloque de numVertices bytes
		file.read((char*)data, m->numVertices * sizeof(char));
		file.close();

		// modificar la coordenada Y de los vértices con los datos de data (*0.05)
		for (int i = 0; i < m->numVertices; i++)
		{
			m->vertices[i].y = int(data[i]) * 0.05 * scale;// *scale;
		}
		delete[]data;
	}
	/* b) Es un mapa de alturas en escala B/N */
	else
	{
		// Crea el mapa de píxeles con los píxeles del mapa de alturas
		PixMap32RGBA pixMap;
		pixMap.load_bmp24BGR(filename);
		if (pixMap.is_null())
			std::cout << "No se pudo cargar el mapa de alturas " << filename << std::endl;

		// Dimensiones
		int w = pixMap.width();
		int h = pixMap.height();

		// Crear la malla
		m = generateGrid(h - 1, w - 1, scale, scale);

		// modificar la coordenada Y de los vértices con los datos de la imagen(*0.05)
		for (int i = 0; i < h; i++)
		{
			for (int j = 0; j < w; j++)
			{
				// La imagen empieza a leerse por abajo a la izquierda, mientras que el Grid empieza por arriba izda.
				m->vertices[w * i + j].y = int(pixMap.data()[w * (h - 1 - i) + j].r) * 0.05 * scale;
			}
		}
	}

	/* Generar normales */
	m->SetNormals();

	return m;
}

IndexMesh* IndexMesh::generateCubemap(GLdouble size)
{
	IndexMesh* m = new IndexMesh();
	m->type = GL_TRIANGLES; // irrelevante, porque no se usa
	m->numVertices = 6 * 4; //para poder texturizar cada cara
	m->numIndices = 36; // 6 caras x 2 triángulos/cara x 3 vértices/triángulo

	/* Array de vértices */
	size /= 2.0;
	m->vertices = new dvec3[m->numVertices]
	{
		// Cara derecha
		{size, size, -size},
		{size, -size, -size},
		{size, size, size},
		{size, -size, size},

		// Cara izquierda
		{-size, size, size},
		{-size, -size, size},
		{-size, size, -size},
		{-size, -size, -size},

		// Cara superior
		{size, size, -size},
		{size, size, size},
		{-size, size, -size},
		{-size, size, size},


		// Cara inferior
		{size, -size, size},
		{size, -size, -size},
		{-size, -size, size},
		{-size, -size, -size},

		// Cara trasera
		{-size, size, -size},
		{-size, -size, -size},
		{size, size, -size},
		{size, -size, -size},

		// Cara frontal
		{size, size, size},
		{size, -size, size},
		{-size, size, size},
		{-size, -size, size},
	};

	/* Índices para los triángulos */
	m->indices = new GLuint[m->numIndices]
	{
		// Derecha
		0, 1, 2,
		2, 1, 3,

		// Izquierda
		4, 5, 6,
		6, 5, 7,

		// Arriba
		8, 9, 10,
		10, 9, 11,

		// Abajo
		12, 13, 14,
		14, 13, 15,

		// Fondo
		16, 17, 18,
		18, 17, 19,

		// Frente
		20, 21, 22,
		22, 21, 23,
	};

	/* No necesita colores, coordenadas de textura ni normales */

	return m;
}

IndexMesh* IndexMesh::generateTessGrid(GLint filas, GLint columnas, GLdouble tamFila, GLdouble tamColumna)
{
	IndexMesh* m = new IndexMesh();
	m->type = GL_PATCHES;
	m->numVertices = (filas + 1) * (columnas + 1);
	m->numIndices = filas * columnas * 4; //ahorramos algunos índices

	/* Lista de vértices (igual que en IndexMesh) */
	m->vertices = new dvec3[m->numVertices];
	GLuint k = 0; //var. auxiliar
	GLdouble iniZ = -filas / 2.0 * tamFila;
	for (int i = 0; i < filas + 1; i++)
	{
		GLdouble iniX = -columnas / 2.0 * tamColumna;
		for (int j = 0; j < columnas + 1; j++)
		{
			m->vertices[k] = { iniX + tamColumna * j, 0, iniZ + tamFila * i };
			k++;
		}
	}

	/* Lista de colores */
	m->colores = new dvec4[m->numVertices];
	for (int i = 0; i < m->numVertices; i++)
		m->colores[i] = dvec4(0.8, 0.8, 0.8, 1);

	/* Lista de parches (índices) */
	m->indices = new GLuint[m->numIndices];
	k = 0;
	for (int i = 0; i < filas; i++)
	{
		for (int j = 0; j < columnas; j++)
		{
			// Empezando por el vértice de arriba izq. y en sentido antihorario
			m->indices[k] = i * (columnas + 1) + j;
			m->indices[k + 1] = (i + 1) * (columnas + 1) + j;
			m->indices[k + 2] = (i + 1) * (columnas + 1) + j + 1;
			m->indices[k + 3] = i * (columnas + 1) + j + 1;

			k += 4;
		}
	}

	/* Coordenadas de textura (igual que el grid normal) */
	m->texCoords = new dvec2[m->numVertices];
	GLdouble texIncrX = 1.0 / columnas;
	GLdouble texIncrZ = 1.0 / filas;
	k = 0;
	for (int i = 0; i < filas + 1; i++)
	{
		for (int j = 0; j < columnas + 1; j++)
		{
			m->texCoords[k] = { texIncrX * j , 1 - texIncrZ * i };
			k++;
		}
	}

	/* Lista de vectores normales */
	m->normales = new dvec3[m->numVertices];
	for (int i = 0; i < m->numVertices; i++)
		m->normales[i] = { 0,1, 0 };


	return m;
}

#include "Mesh.h"

#include <iostream>
#include <fstream>
#include "Utils.h"
#include "Pixmap32RGBA.h"

using namespace glm;

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
}

void Mesh::drawInstanced(GLuint numInstances)
{
	// 1) Activar los arrays que vamos a usar y pasarlos a la GPU
	enableArrays();

	// 2) Dibuja los arrays pasados a la GPU con la primitiva dada (type)
	glDrawArraysInstanced(type, 0, numVertices, numInstances);

	// 3) Volver a dejarlo todo como estaba
	disableArrays();
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

void IndexMesh::drawInstanced(GLuint numInstances)
{
	// 1) Activar los arrays que vamos a usar y pasarlos a la GPU
	enableArrays();

	// 2) Dibuja los arrays pasados a la GPU con la primitiva dada (type)
	glDrawElementsInstanced(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, indices, numInstances);

	// 3) Volver a dejarlo todo como estaba
	disableArrays();
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

IndexMesh* IndexMesh::generateSphere(GLdouble size, GLuint subdivisions, bool textured)
{
	IndexMesh* m = new IndexMesh();
	m->type = GL_TRIANGLES; 
	m->numVertices = subdivisions * (subdivisions - 1)
		+ 2; // vértice superior e inferior
	m->numIndices = subdivisions * 2 //triángulos de arriba y de abajo
		+ ((subdivisions - 2) * subdivisions * 2); // triángulos del resto del cuerpo
	m->numIndices *= 3; // 3 índices para cada triángulo


	/* Array de  vértices */
	m->vertices = new dvec3[m->numVertices];
	m->vertices[0] = { 0, size, 0 }; //tapas
	m->vertices[m->numVertices - 1] = { 0, -size, 0 };

	GLdouble angleIncr = 2 * PI / subdivisions;
	GLdouble angleIncrY = PI / subdivisions;
	GLdouble initialAngle = PI / 2; // 90 grados
	// Plano cenital
	int k = 1;
	for(int i = 1; i < subdivisions; i++)
	{
		GLdouble posY = size * sin(initialAngle + angleIncrY * i);
		GLdouble radius = size * cos(initialAngle - angleIncrY * i);
		// Dibujar cada uno de los cortes cenitales en Y = posY
		for(int j = 0; j < subdivisions; j++)
		{
			GLdouble posX = -radius * cos(initialAngle + angleIncr * j);
			GLdouble posZ = radius * sin(initialAngle + angleIncr * j);
			m->vertices[k] = { posX,posY,posZ };
			//std::cout << "V" << k << ": (" << posX << ", " << posY << ", " << posZ << ")" << std::endl;
			k++;
		}
	}

	/* Array de colores */
	m->colores = new dvec4[m->numVertices];
	for (int i = 0; i < m->numVertices; i++)
	{
		m->colores[i] = dvec4(0.8, 0.8, 0.8, 1);
	}


	/* Especificar los triángulos (array de índices) */
	m->indices = new GLuint[m->numIndices];
	for (int i = 0; i < m->numIndices; i++)
		m->indices[i] = 0;
	// "Tapa" de arriba"
	for(int i = 0; i < subdivisions; i++)
	{
		m->indices[i * 3] = 0;
		m->indices[i * 3 + 1] = i + 1;
		m->indices[i * 3 + 2] = i + 2;
	}
	m->indices[(subdivisions - 1) * 3 + 2] = 1; //cerrar el círculo

	// Cuerpo de la esfera
	int count = 3 * subdivisions;
	for(int i = 1; i < subdivisions - 1; i++) // < subdivisions - 1
	{
		GLuint v1, v2, v3, v4;
		// Pintar cuadrados en cada nivel de la esfera
		for (int j = 1; j <= subdivisions; j++)
		{
			//  v1  v3
			//  |  / |
			//  | /  |
			//  v2   v4
			v1 = subdivisions * (i - 1) + j;
			v2 = subdivisions * i + j;
			v3 = subdivisions * (i - 1) + j + 1;
			v4 = subdivisions * i + j + 1;
			if(j == subdivisions) // un caso especial, habría mejor forma de hacerlo
			{
				v3 -= subdivisions;
				v4 -= subdivisions;
			}

			// Primer triángulo del cuadrado
			m->indices[count] = v1;
			m->indices[count + 1] = v2;
			m->indices[count + 2] = v3;
			count += 3;

			// Segundo triángulo del cuadrado
			m->indices[count] = v3;
			m->indices[count + 1] = v2;
			m->indices[count + 2] = v4;
			count += 3;
		}
	}

	// "Tapa" de abajo
	for (int i = 0; i < subdivisions; i++)
	{
		m->indices[count] = m->numVertices - subdivisions + i - 1;
		m->indices[count + 1] = m->numVertices - 1;
		m->indices[count + 2] = m->numVertices - subdivisions + i;
		count += 3;
	}
	m->indices[count - 1] = m->numVertices - subdivisions - 1; //cerrar el círculo


	/* Coordenadas de textura */
	if(textured)
	{
		m->texCoords = new dvec2[m->numVertices];
		// Niveles (Y)
		for(int i = 0; i < subdivisions - 1; i++)
		{
			for(int j = 0; j < subdivisions; j++)
			{
				GLdouble texX = 0.5 + (1.0 / subdivisions) * j;
				if (texX >= 1) // se podría hacer mejor
					texX -= 1.0;
				GLdouble texY = 1 - (1.0 / (subdivisions + 1) * i);
				m->texCoords[i * subdivisions + j + 1] = { texX, texY };
			}
		}
		// Tapas
		m->texCoords[0] = { 0.5, 1 };
		m->texCoords[m->numVertices - 1] = { 0.5, 0 };
	}

	/* Normales */
	m->SetNormals();

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

	/* Lista de vectores normales */
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

// - - - - - - - - - - - - - - - - 

void TessMesh::draw()
{
	// Activa los arrays de vértices, colores, texturas... y el de índices
	Mesh::enableArrays();
	if (indices != nullptr)
	{
		glEnableClientState(GL_INDEX_ARRAY);
		glIndexPointer(GL_UNSIGNED_INT, 0, indices);
	}

	// Dibuja los parches definidos por la tabla de índices
	glDrawElements(GL_PATCHES, numIndices, GL_UNSIGNED_INT, indices);

	// Dejarlo todo como estaba
	glDisableClientState(GL_INDEX_ARRAY);
	Mesh::disableArrays();
}

TessMesh* TessMesh::generateTessGrid(GLint filas, GLint columnas, GLdouble tamFila, GLdouble tamColumna)
{
	TessMesh* m = new TessMesh();
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

	/* Coordenadas de textura -> se podría meter en el mismo bucle que los vértices */
	//m->texCoords = new dvec2[m->numVertices];
	//GLdouble texIncrX = 1.0 / columnas;
	//GLdouble texIncrZ = 1.0 / filas;
	//k = 0;
	//for (int i = 0; i < filas + 1; i++)
	//{
	//	for (int j = 0; j < columnas + 1; j++)
	//	{
	//		m->texCoords[k] = { texIncrX * j , 1 - texIncrZ * i };
	//		k++;
	//	}
	//}

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

	/* Lista de vectores normales */
	m->normales = new dvec3[m->numVertices];
	for (int i = 0; i < m->numVertices; i++)
		m->normales[i] = { 0,1, 0 };


	return m;
}

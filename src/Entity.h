#ifndef __ENTITY__
#define __ENTITY__

#include "Mesh.h"
#include "Texture.h"

#include <glm.hpp>
#include <freeglut.h>

#include <iostream>

/**
 * Imprime por consola una matriz cuadrada de una forma cómoda para la vista.
 *
 * Los datos deben estar contiguos, sin espaciamiento entre ellos, y 
 * en orden 'column-major' (por columnas, y no por filas)
 *
 * @param matrixPtr Puntero al primer dato de la matriz
 */
template<typename T = double, int size = 4>
static void PrintMatrix(void* matrixPtr) // ESTÁ MAL-> FILAS Y COLUMNAS CAMBIADAS
{
	// Filas
	for (int i = 0; i < size; i++)
	{
		// Columnas
		std::cout << "(";
		for (int j = 0; j < size; j++)
		{
			std::cout << ((T*)matrixPtr)[j * size + i] << " ";
		}
		std::cout << ")\n";
	}
	std::cout << "\n";
}

// Imprime el nombre y contenido de una variable por pantalla
#define NOMBRE(VAR) std::cout << #VAR << std::endl;

class Entity
{
public:
	/* Constructora por defecto */
	Entity() : m_mesh(nullptr), modelMat(1.0) // Pone la matriz de modelado a la matriz identidad de grado 4 (1 0 0 0 / 0 1 0 0 ...)
	{
		//PrintMatrix<double, 4>(&modelMat);
		//NOMBRE(modelMat);
	}
	
	
	virtual ~Entity() { };

	/* La entidad pinta su malla, cargando antes la matriz M*V para 
	que la posición de la cámara afecte a la posición los vértices que van a pintarse. (espacio local -> global)
	Si 'viewMat' fuera la identidad, estos 2 espacios coincidirían ¿? */
	virtual void render(glm::dmat4 const& viewMat);

	// Actualizar
	virtual void update(GLuint timeElapsed);

	/* Mueve la entidad a la posición dada */
	void setPosition(glm::dvec3 pos);

	/* Rota la entidad en el eje dado, la cantidad de grados especificada (en radianes) */
	void rotate(GLdouble alpha, glm::dvec3 axis);

protected:
	/* Malla/s que usará la entidad para pintarse */
	Mesh* m_mesh;

	/* Matriz de modelado de la entidad; recoge las transformaciones 
	(traslaciones, rotaciones, escalados) hechas a la entidad */
	glm::dmat4 modelMat;

	/* Textura utilizada por la entidad */
	Texture m_texture;

	//Mesh* mesh = nullptr;
	//Mesh* mesh2 = nullptr;
	//Texture texture;
	//Texture texture2;


	//virtual void draw();
	//virtual void setMvM(glm::dmat4 const& modelViewMat);

	////Método auxiliar
	//virtual void drawTexture(GLenum face, Mesh* meshUsed, Texture* textureUsed);
};

// - - - - - - - - - - - -

class EjesRGB : public Entity
{
public:
	EjesRGB(GLdouble l);
	~EjesRGB() { };
	//void render() override;
};

// - - - - - - - - - - - - 

class Poligono : public Entity
{
public:
	Poligono(GLint sides, GLdouble size, bool relleno = false, std::string textureName = "");
	~Poligono() { };
	void render(glm::dmat4 const& viewMat) override;
};

// - - - - - - - - - - - - 

class Cubo : public Entity
{
public:
	Cubo(GLdouble size);
	~Cubo() { };
	void render(glm::dmat4 const& viewMat) override;
};

// - - - - - - - - - - - - 

class Esfera : public Entity
{
public:
	Esfera(GLuint subdivisiones);
	~Esfera() { };
	void render(glm::dmat4 const& viewMat) override;
};

// - - - - - - - - - - - - 

class Grid : public Entity
{
public:
	Grid(GLuint filas, GLuint columnas, GLdouble tamFila, GLdouble tamColumna);
	~Grid() { };
	void render(glm::dmat4 const& viewMat) override;
};

#endif
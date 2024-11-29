#ifndef __ENTITY__
#define __ENTITY__

#include "Mesh.h"

#include <glm.hpp>
#include <freeglut.h>

#include <iostream>

// Imprime el nombre y contenido de una variable por pantalla
#define NOMBRE(VAR) std::cout << #VAR << std::endl;

class Entity
{
public:
	/* Constructora por defecto */
	Entity() : modelMat(1.0) // Pone la matriz de modelado a la matriz identidad de grado 4 (1 0 0 0 / 0 1 0 0 ...)
	{
		PrintMatrix<double, 4>(&modelMat);
		NOMBRE(modelMat);
	} 
	
	
	virtual ~Entity() { };

	/* La entidad pinta su malla, cargando antes la matriz M*V para 
	que la posición de la cámara afecte a la posición los vértices que van a pintarse. (espacio local -> global)
	Si 'viewMat' fuera la identidad, estos 2 espacios coincidirían ¿? */
	virtual void render(glm::dmat4 const& viewMat);

	// Actualizar
	virtual void update(GLuint timeElapsed);

	/**
	 * Imprime por consola una matriz cuadrada de una cómoda para la vista.
	 *
	 * Los datos deben estar contiguos, sin espaciamiento entre ellos
	 *
	 * @param matrixPtr Puntero al primer dato de la matriz
	 * @param type El tipo de datos que la matriz contiene
	 * @param matrixSize El grado de la matriz
	 */
	template<typename T, int size>
	static void PrintMatrix(void* matrixPtr)
	{
		for(int i = 0; i < size; i++)
		{
			std::cout << "(";
			for (int j = 0; j < size; j++)
			{
				std::cout << ((T*)matrixPtr)[i * size + j] << " ";
			}
			std::cout << ")\n";
		}
		std::cout << "\n";
	}

protected:
	/* Malla/s que usará la entidad para pintarse */
	Mesh* m_mesh;

	/* Matriz de modelado de la entidad; recoge las transformaciones 
	(traslaciones, rotaciones, escalados) hechas a la entidad */
	glm::dmat4 modelMat;

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
	Poligono(GLint sides, bool relleno);
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

#endif
#ifndef __ENTITY__
#define __ENTITY__

#include "Mesh.h"
#include "Texture.h"
#include "Material.h"

#include <glm.hpp>
#include <freeglut.h>

#include <iostream>
#include "Utils.h"


class Entity
{
public:
	/* Constructora por defecto */
	Entity() : m_mesh(nullptr), modelMat(1.0) // Pone la matriz de modelado a la matriz identidad de grado 4 (1 0 0 0 / 0 1 0 0 ...)
	{
		//PrintMatrix<double, 4>(&modelMat);
		//NOMBRE(modelMat);
	}
	
	/* Destructora virtual */
	virtual ~Entity() { delete m_mesh; };

	/* La entidad pinta su malla, cargando antes la matriz M*V para 
	que la posición de la cámara afecte a la posición los vértices que van a pintarse. (espacio local -> global)
	Si 'viewMat' fuera la identidad, estos 2 espacios coincidirían ¿? */
	virtual void render(glm::dmat4 const& viewMat);

	// Actualizar
	virtual void update(GLuint timeElapsed);

	/* Mueve la entidad a la posición dada */
	void setPosition(glm::dvec3 pos);

	/* Suma el vector de tralación dado a la entidad, usando un sistema de referencia dado */
	void translate(glm::dvec3 transVector, ReferenceSystem refSys = GLOBAL);

	/* Rota la entidad en el eje dado, la cantidad de grados especificada (en radianes) */
	void rotate(GLdouble alpha, glm::dvec3 axis, ReferenceSystem refSys = GLOBAL);

	/* Escala la entidad la cantidad dada en cada eje */
	void scale(glm::dvec3 scale);

	// Setters
	/* Establece la textura (ya creada) que usará la entidad */
	inline void setTexture(Texture const& tex) { m_texture = tex; };

	/* Establece el material (ya creado) que usará la entidad */
	inline void setMaterial(Material const& mat) { m_material = mat; };

protected:
	/* Malla/s que usará la entidad para pintarse */
	Mesh* m_mesh;

	/* Matriz de modelado de la entidad; recoge las transformaciones 
	(traslaciones, rotaciones, escalados) hechas a la entidad */
	glm::dmat4 modelMat;

	/* Textura utilizada por la entidad */
	Texture m_texture;

	/* Material usado por la entidad */
	Material m_material;

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
	Poligono(GLint sides, GLdouble size, bool relleno = false);
	~Poligono() { };
	void render(glm::dmat4 const& viewMat) override;
};

// - - - - - - - - - - - - 

class Cubo : public Entity
{
public:
	Cubo(GLdouble size, bool textured, bool equalFaces = true);
	~Cubo() { };
	void render(glm::dmat4 const& viewMat) override;
	void update(GLuint timeElapsed) override;
};

// - - - - - - - - - - - - 

class Esfera : public Entity
{
public:
	Esfera(GLdouble size, GLuint subdivisions = 20, bool textured = false);
	~Esfera() { };
	void render(glm::dmat4 const& viewMat) override;
	void update(GLuint timeElapsed) override;
};

// - - - - - - - - - - - - 

class Grid : public Entity
{
public:
	Grid(GLuint filas, GLuint columnas, GLdouble tamFila, GLdouble tamColumna);
	~Grid() { };
	void render(glm::dmat4 const& viewMat) override;
};

// - - - - - - - - - - - - 

class Terrain : public Entity
{
public:
	/* Crea un terreno con la información de 'filename' y la textura de 'textureName' */
	Terrain(std::string filename, GLdouble scale);
	~Terrain() { };
	void render(glm::dmat4 const& viewMat) override;
};

#endif
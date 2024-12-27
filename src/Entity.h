#ifndef __ENTITY__
#define __ENTITY__

class Mesh;
class Texture;
#include "Material.h"

#include <glm.hpp>


#include <iostream>
#include "Utils.h"


class Entity
{
public:
	/* Constructora por defecto */
	Entity() : m_mesh(nullptr), m_texture(nullptr), modelMat(1.0) // Pone la matriz de modelado a la matriz identidad de grado 4 (1 0 0 0 / 0 1 0 0 ...)
	{
		//PrintMatrix<double, 4>(&modelMat);
		//NOMBRE(modelMat);
	}
	
	/* Destructora virtual */
	virtual ~Entity() { delete m_mesh; };

	/* La entidad pinta su malla en función de la matriz de vista, cargando antes su textura y 
	material correspondiente */
	virtual void render(glm::dmat4 const& viewMat);

	// Actualizar
	virtual void update(GLuint timeElapsed) { }

	// Transformaciones afines
	/* Suma el vector de tralación dado a la entidad, usando un sistema de referencia dado */
	void translate(glm::dvec3 transVector, ReferenceSystem refSys = GLOBAL);

	/* Rota la entidad en el eje dado, la cantidad de grados especificada (en radianes) */
	void rotate(GLdouble alpha, glm::dvec3 axis, ReferenceSystem refSys = GLOBAL);

	/* Escala la entidad la cantidad dada en cada eje */
	void scale(glm::dvec3 scale);

	// Setters
	/* Mueve la entidad a la posición dada */
	void setPosition(glm::dvec3 pos);

	/* Establece la malla (ya creada) que usará la entidad */
	void setMesh(std::string meshID);

	/* Establece la textura (ya creada) que usará la entidad */
	void setTexture(std::string textureID);

	/* Establece el material (ya creado) que usará la entidad */
	void setMaterial(std::string materialID);

	// Getters
	/* Devuelve la matriz de modelado de la entidad */
	const glm::dmat4& getModelMat() { return modelMat; }

protected:
	/* Malla/s que usará la entidad para pintarse */
	Mesh* m_mesh;

	/* Matriz de modelado de la entidad; recoge las transformaciones 
	(traslaciones, rotaciones, escalados) hechas a la entidad */
	glm::dmat4 modelMat;

	/* Textura utilizada por la entidad */
	Texture* m_texture;

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
};

// - - - - - - - - - - - - 

class Cubo : public Entity
{
public:
	Cubo(GLdouble size, bool textured, bool equalFaces = true);
	~Cubo() { };
	void update(GLuint timeElapsed) override;
};

// - - - - - - - - - - - - 

class Esfera : public Entity
{
public:
	Esfera(GLdouble size, GLuint subdivisions = 20, bool textured = false);
	~Esfera() { };
	void update(GLuint timeElapsed) override;
};

// - - - - - - - - - - - - 

class Grid : public Entity
{
public:
	Grid(GLuint filas, GLuint columnas, GLdouble tamFila, GLdouble tamColumna);
	~Grid() { };
};

// - - - - - - - - - - - - 

class Terrain : public Entity
{
public:
	/* Crea un terreno con la información de 'filename' y la textura de 'textureName' */
	Terrain(std::string filename, GLdouble scale);
	~Terrain() { };
};

#endif
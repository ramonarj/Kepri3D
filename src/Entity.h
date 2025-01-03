#ifndef __ENTITY__
#define __ENTITY__

class Mesh;
class Texture;
class Shader;
#include "Material.h"

#include <glm.hpp>

#include <iostream>
#include "Utils.h"


class Entity
{
public:
	/* Constructora por defecto */
	Entity() : m_parent(nullptr), m_active(true), m_mesh(nullptr), m_texture(nullptr), 
		modelMat(1.0), m_shader(nullptr), m_specMap(nullptr)
		// Pone la matriz de modelado a la matriz identidad de grado 4 (1 0 0 0 / 0 1 0 0 ...)
	{
		//PrintMatrix<double, 4>(&modelMat);
		//NOMBRE(modelMat);
	}
	
	/* Destructora virtual */
	virtual ~Entity();

	/* La entidad pinta su malla en funci�n de la matriz de vista, cargando antes su textura y 
	material correspondiente */
	virtual void render(glm::dmat4 const& viewMat);

	// Actualizar
	virtual void update(GLuint deltaTime);

	// Transformaciones afines
	/* Suma el vector de tralaci�n dado a la entidad, usando un sistema de referencia dado */
	void translate(glm::dvec3 transVector, ReferenceSystem refSys = GLOBAL);

	/* Rota la entidad en el eje dado, la cantidad de grados especificada (en radianes) */
	void rotate(GLdouble alpha, glm::dvec3 axis, ReferenceSystem refSys = GLOBAL);

	/* Escala la entidad la cantidad dada en cada eje */
	void scale(glm::dvec3 scale);

	// Setters
	/* Activa / desactiva la entidad */
	inline void setActive(bool b) { m_active = b; }

	/* Mueve la entidad a la posici�n dada */
	void setPosition(glm::dvec3 pos);

	/* Establece el padre de la entidad */
	void setParent(Entity* e);

	/* Establece la malla (ya creada) que usar� la entidad */
	void setMesh(const std::string& meshID);

	/* Establece la textura (ya creada) que usar� la entidad */
	void setTexture(const std::string& textureID);

	/* Establece el material (ya creado) que usar� la entidad */
	void setMaterial(const std::string& materialID);

	/* Establece el shader (ya creado) que usar� la entidad */
	void setShader(const std::string& shaderID);

	/* Establece el mapa especular (textura en B/N) que se usar� para los brillos especulares de la textura principal */
	void setSpecularMap(const std::string& textureID);

	// Getters
	/* Devuelve la posici�n de la entidad */
	const glm::dvec3& getPosition() const { return modelMat[3]; }

	/* Devuelve la matriz de modelado de la entidad */
	const glm::dmat4& getModelMat() { return modelMat; }

	/* Devuelve el shader que usa la entidad */
	const Shader* getShader() const { return m_shader; }

	/* Devuelve 'true' si la entidad debe pintarse, false e.o.c. */
	inline bool isActive() const { return m_active; }

	// Ejes locales de la entidad
	inline glm::dvec3 right() { return modelMat[0]; }
	inline glm::dvec3 up() { return modelMat[1]; }
	inline glm::dvec3 forward() { return modelMat[2]; }


protected:
	/* Indica si la entidad debe actualizarse y pintarse */
	bool m_active;

	/* Hijos de la entidad */
	std::vector<Entity*> m_children;

	/* Padre de la entidad */
	Entity* m_parent;

	/* Malla/s que usar� la entidad para pintarse */
	Mesh* m_mesh;

	/* Matriz de modelado de la entidad; recoge las transformaciones 
	(traslaciones, rotaciones, escalados) hechas a la entidad */
	glm::dmat4 modelMat;

	/* Textura utilizada por la entidad */
	Texture* m_texture;

	/* Material usado por la entidad */
	Material m_material;

	/* Shader usado por la entidad */
	Shader* m_shader;

	/* Mapa especular usado para la entidad */
	Texture* m_specMap;

	//Mesh* mesh = nullptr;
	//Mesh* mesh2 = nullptr;
	//Texture texture;
	//Texture texture2;


	//virtual void draw();
	//virtual void setMvM(glm::dmat4 const& modelViewMat);

	////M�todo auxiliar
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
	void update(GLuint timeElapsed) override;
	void render(glm::dmat4 const& viewMat) override;
private:
	int timeLoc;
};

// - - - - - - - - - - - - 

class Terrain : public Entity
{
public:
	/* Crea un terreno con la informaci�n de 'filename' y la textura de 'textureName' */
	Terrain(std::string filename, GLdouble scale);
	~Terrain() { };
};

#endif
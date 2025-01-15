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
	Entity();
	Entity(const std::string& name);
	
	/* Destructora virtual */
	virtual ~Entity();

	/* La entidad pinta su malla en función de la matriz de vista, cargando antes su textura y 
	material correspondiente */
	virtual void render(glm::dmat4 const& viewMat);

	// Actualizar
	virtual void update(GLuint deltaTime);

	// Transformaciones afines
	/* Suma el vector de tralación dado a la entidad, usando un sistema de referencia dado */
	void translate(const glm::dvec3& transVector, ReferenceSystem refSys = GLOBAL);

	/* Rota la entidad en el eje dado, la cantidad de grados especificada (en radianes) */
	void rotate(GLdouble alpha, const glm::dvec3& axis, ReferenceSystem refSys = GLOBAL);

	/* Escala la entidad la cantidad dada en cada eje */
	void scale(const glm::dvec3& scale);

	// Setters
	/* Activa / desactiva la entidad */
	inline void setActive(bool b) { m_active = b; }

	/* Establece el nombre de la entidad */
	inline void setName(const std::string& name) { m_name = name; }

	/* Mueve la entidad a la posición dada */
	void setPosition(const glm::dvec3& pos);

	/* Establece el padre de la entidad */
	void setParent(Entity* e);

	/* Establece la malla (ya creada) que usará la entidad */
	void setMesh(const std::string& meshID);

	/* Establece la textura (ya creada) que usará la entidad */
	void setTexture(const std::string& textureID);

	/* Establece el material (ya creado) que usará la entidad */
	void setMaterial(const std::string& materialID);

	/* Establece el shader (ya creado) que usará la entidad */
	void setShader(const std::string& shaderID);

	/* Establece el mapa especular (textura en B/N) que se usará para los brillos especulares de la textura principal */
	void setSpecularMap(const std::string& textureID);

	// Getters
	/* Devuelve el nombre de la entidad */
	const std::string& getName() const { return m_name; }

	/* Devuelve la posición de la entidad */
	const glm::dvec3& getPosition() const { return modelMat[3]; }

	/* Devuelve la matriz de modelado de la entidad */
	const glm::dmat4& getModelMat() const { return modelMat; }

	/* Devuelve el material que usa la entidad */
	const Material* getMaterial() const { return &m_material; }

	/* Devuelve el shader que usa la entidad */
	const Shader* getShader() const { return m_shader; }

	/* Devuelve 'true' si la entidad debe pintarse, false e.o.c. */
	inline bool isActive() const { return m_active; }

	// Ejes locales de la entidad
	inline glm::dvec3 right() { return modelMat[0]; }
	inline glm::dvec3 up() { return modelMat[1]; }
	inline glm::dvec3 forward() { return modelMat[2]; }

private:
	/* Establece los valores por defecto de la entidad */
	void defaultValues();

protected:
	/* Indica si la entidad debe actualizarse y pintarse */
	bool m_active;

	/* Nombre de la entidad */
	std::string m_name;

	/* Hijos de la entidad */
	std::vector<Entity*> m_children;

	/* Padre de la entidad */
	Entity* m_parent;

	/* Malla/s que usará la entidad para pintarse */
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

class Toro : public Entity
{
public:
	Toro(GLdouble radioExterior, GLdouble radioInterior, GLuint anillos, GLuint lineas);
	~Toro() { };
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
	/* Crea un terreno vacío */
	Terrain();

	/* Crea el terreno con un archivo RAW de alturas (256 x 256) */
	void loadRAW(const std::string& rawFile, GLdouble scale = 1.0);

	/* Crea el terreno con un mapa de alturas BMP (tendrá tantos vértices como píxeles la imagen) */
	void loadHeightMap(const std::string& heightFile, GLdouble scale = 1.0);

	~Terrain() { };
};

// - - - - - - - - - - - - 

class Skybox : public Entity
{
public:
	/* Crea un skybox con la textura tipo Cubemap previamente cargada */
	Skybox(const std::string& cubemapTextureID);
	~Skybox() { };

	void render();
};

// - - - - - - - - - - - - 

class CuboMultitex : public Entity
{
public:
	CuboMultitex(GLdouble size);
	~CuboMultitex() { };

	void render(glm::dmat4 const& viewMat) override;

	void setSecondTex(const std::string& textureID);
private:
	Texture* secondTex;
};

// - - - - - - - - - - - - 

class CuboSpecmap : public Entity
{
public:
	CuboSpecmap(GLdouble size);
	~CuboSpecmap() { };

	void render(glm::dmat4 const& viewMat) override;
};

// - - - - - - - - - - - - 

class Hierba : public Entity
{
public:
	Hierba(GLdouble widh, GLdouble height);
	~Hierba() { };

	void render(glm::dmat4 const& viewMat) override;
};

#endif
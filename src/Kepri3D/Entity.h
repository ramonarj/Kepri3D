#ifndef __ENTITY__
#define __ENTITY__

class Texture;
class Shader;
class Component;
class Renderer;
class Collider;
class Material;

class Vertexbuffer;
class Elementbuffer;
class VertexArray;

#include <glm.hpp>
#include <glew.h>
#include <iostream>
#include "Utils.h"

class Entity
{
public:
	/* Constructora por defecto */
	Entity(const std::string& name = "- Entidad s/n - ");
	Entity(std::vector<Component*> comps, const std::string& name = "- Entidad s/n - ");
	
	/* Destructora virtual */
	virtual ~Entity();

	/* Añade un componente a la entidad */
	void addComponent(Component* c);

	/* Método render que usa el fixed pipeline; pinta su malla en función de la matriz de vista, cargando antes su textura y 
	material correspondiente */
	virtual void render(glm::dmat4 const& viewMat);

	/* Método render que usa shaders; necesario activar el shader y pasar los uniforms correspondientes antes
	de llamar a la función */
	virtual void render();
	virtual void render(Shader* sh);

	// Actualizar
	virtual void update(float deltaTime);
	virtual void fixedUpdate(float fixedTime);

	// Transformaciones afines
	/* Suma el vector de tralación dado a la entidad, usando un sistema de referencia dado */
	void translate(const glm::dvec3& transVector, ReferenceSystem refSys = GLOBAL);

	/* Rota la entidad en el eje dado, la cantidad de grados especificada (en radianes) */
	void rotate(GLdouble alpha, const glm::dvec3& axis, ReferenceSystem refSys = GLOBAL);

	/* Escala la entidad la cantidad dada en cada eje */
	void scale(const glm::dvec3& scale);

	void lookAt(const glm::dvec3& point, const glm::dvec3& up = { 0, 1, 0 });

	// - - - - Setters - - - - //
	inline void setActive(bool b) { m_active = b; }
	inline void setName(const std::string& name) { m_name = name; }
	void setPosition(const glm::dvec3& pos);
	void setParent(Entity* e);
	void setMesh(const std::string& meshID);
	void setTexture(const std::string& textureID);
	void setMaterial(const std::string& materialID);
	void setShader(const std::string& shaderID);

	// - - - - Getters - - - - //
	inline bool isActive() const { return m_active; }
	inline const std::string& getName() const { return m_name; }
	inline glm::dvec3 getPosition() const { return modelMat[3]; }
	inline Entity* getParent() { return m_parent; }
	Material* getMaterial() const;
	const Shader* getShader() const;
	inline Renderer* getRenderer() const { return m_renderer; }
	inline std::vector<Component*> getComponents() { return m_componentes; }
	inline std::vector<Entity*> getChildren() { return m_children; }
	inline const glm::dmat4& getModelMat() const { return modelMat; }

	// Ejes locales de la entidad
	inline glm::dvec3 right() { return modelMat[0]; }
	inline glm::dvec3 up() { return modelMat[1]; }
	glm::dvec3 forward(); // TODO: hacer lo mismo con right() y up()

	/* Devuelve un componente del tipo requerido */
	template<typename T>
	T* getComponent();

private:
	/* Establece los valores por defecto de la entidad */
	void defaultValues();
protected:
	/* Indica si la entidad debe actualizarse y pintarse */
	bool m_active;

	/* Nombre de la entidad */
	std::string m_name;

	/* Lista de componentes */
	std::vector<Component*> m_componentes;

	/* Componente renderer (acceso directo) */
	Renderer* m_renderer;

	/* Componente collider (acceso directo) */
	Collider* m_collider;

	/* Hijos de la entidad */
	std::vector<Entity*> m_children;

	/* Padre de la entidad */
	Entity* m_parent;

	/* Matriz de modelado de la entidad; recoge las transformaciones 
	(traslaciones, rotaciones, escalados) hechas a la entidad */
	glm::dmat4 modelMat;

	/* Manda la matriz de modelado (combinada con la del padre) al shader */
	void sendModelMat(Shader* sh);
};

template<typename T>
T* Entity::getComponent()
{
	for(Component* c: m_componentes)
	{
		if (dynamic_cast<T*>(c) != nullptr)
			return dynamic_cast<T*>(c);
	}
	return nullptr;
}

// - - - - - - - - - - - -

class EjesRGB : public Entity
{
public:
	EjesRGB(GLdouble l);
	~EjesRGB() { };
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
	Cubo(GLdouble size = 1, bool equalFaces = true);
	~Cubo() { };
};

// - - - - - - - - - - - - 

class Esfera : public Entity
{
public:
	Esfera(GLdouble radio = 1, GLuint paralelos = 10, GLuint meridianos = 20);
	~Esfera() { };
};

// - - - - - - - - - - - - 

class Cilindro : public Entity
{
public:
	Cilindro(GLdouble radio = 1, GLdouble altura = 2, GLuint lados = 20);
	~Cilindro() { };
};

// - - - - - - - - - - - - 

class Cono : public Entity
{
public:
	Cono(GLdouble radio = 1, GLdouble altura = 2, GLuint lados = 20);
	~Cono() {};
};

// - - - - - - - - - - - - 

class Toro : public Entity
{
public:
	Toro(GLdouble radioExterior = 1, GLdouble radioInterior = 0.5, GLuint anillos = 15, GLuint lineas = 8);
	~Toro() { };
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
	/* Crea un terreno vacío */
	Terrain();

	/* Crea el terreno con un archivo RAW de alturas (256 x 256) */
	void loadRAW(const std::string& rawFile, GLdouble scale = 1.0);

	/* Crea el terreno con un mapa de alturas BMP (tendrá tantos vértices como píxeles la imagen) */
	void loadHeightMap(const std::string& heightFile, GLdouble scale = 1.0);

	~Terrain() { };
};

// - - - - - - - - - - - - 

class TessTerrain : public Entity
{
public:
	/* Crea un terreno vacío */
	TessTerrain(GLuint filas, GLuint columnas, GLdouble tamFila, GLdouble tamColumna);
	~TessTerrain() { };
};

// - - - - - - - - - - - - 

class Skybox : public Entity
{
public:
	/* Crea un skybox con la textura tipo Cubemap previamente cargada */
	Skybox(const std::string& cubemapTextureID);
	~Skybox() { };
};

// - - - - - - - - - - - - 

class Billboard : public Entity
{
public:
	Billboard(const std::string& textureID, GLfloat width, GLfloat height);
	~Billboard() { };

	inline void setAncla(const glm::vec2& ancla) { this->ancla = ancla; }

	void render() override;
private:
	glm::vec2 ancla;
};

// - - - - - - - - - - - - 

class Hierba : public Entity
{
public:
	Hierba(GLdouble widh, GLdouble height);
	~Hierba() { };

	void render(glm::dmat4 const& viewMat) override;
};

// - - - - - - - - - - - - 

class ClippableEntity : public Entity
{
public:
	ClippableEntity();
	~ClippableEntity() { };

	void render() override;
private:
	std::vector<glm::vec4> planos;
};

// - - - - - - - - - - - - 

class VBOEntity : public Entity
{
public:
	VBOEntity();
	~VBOEntity();

	void render() override;

private:
	GLenum type;
	unsigned int numVerts;
	Vertexbuffer* vbo;
};

// - - - - - - - - - - - - 

class EBOEntity : public Entity
{
public:
	EBOEntity();
	~EBOEntity();

	void render() override;

private:
	GLenum type;
	unsigned int numVerts;
	unsigned int numIndices;
	Vertexbuffer* vbo;
	Elementbuffer* ebo;
};

// - - - - - - - - - - - - 

class VAOEntity : public Entity
{
public:
	VAOEntity();
	~VAOEntity();

	void render() override;

private:
	GLenum type;
	unsigned int numVerts;
	unsigned int numIndices;

	VertexArray* vao;
};

#endif
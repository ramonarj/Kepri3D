#ifndef __ENTITY__
#define __ENTITY__

class Texture;
class Shader;
class Component;
class Renderer;
class Collider;

class Vertexbuffer;
class Elementbuffer;
class VertexArray;

#include "Material.h"

#include <glm.hpp>

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

	/* Establece el material (ya creado) que usará la entidad */
	void setMaterial(const std::string& materialID);

	/* Establece el shader (ya creado) que usará la entidad */
	void setShader(const std::string& shaderID);

	/* Establece la textura principal (diffuse) que usará la entidad */
	void setTexture(const std::string& textureID);
	inline const Texture* getTexture() const { return m_material.getTexture(0); }

	/* Establece la textura secundaria a usar, que se mezclará con la principal en la medida dada */
	void setSecondTex(const std::string& textureID);

	/* Establece el mapa especular (textura en B/N) que se usará para 'reemplazar' la componente specular del material */
	void setSpecularMap(const std::string& textureID);

	/* Establece el mapa de normales (bump map) que se usará para 'reemplazar' las normales de la malla */
	void setNormalMap(const std::string& textureID);

	/* Establece el mapa de desplazamiento (textura en B/N) que desplazará las coordenadas de textura de la malla */
	void setDisplacementMap(const std::string& textureID);

	/* Activa los reflejos para esta entidad, dando el mapa de reflejos (textura en B/N) que indica qué partes
	* son reflectantes, y la textura del Cubemap que reflejará */
	void enableReflections(const std::string& reflectionMapID, const std::string& cubemapID);

	// Getters
	/* Devuelve el nombre de la entidad */
	const std::string& getName() const { return m_name; }

	/* Devuelve la posición de la entidad */
	glm::dvec3 getPosition() const { return modelMat[3]; }

	/* Devuelve la matriz de modelado de la entidad */
	const glm::dmat4& getModelMat() const { return modelMat; }

	/* Devuelve el renderer que usa la entidad */
	Renderer* getRenderer() const { return m_renderer; }

	/* Devuelve el material que usa la entidad */
	const Material* getMaterial() const { return &m_material; }

	/* Devuelve el shader que usa la entidad */
	const Shader* getShader() const { return m_material.getShader(); }

	/* Devuelve 'true' si la entidad debe pintarse, false e.o.c. */
	inline bool isActive() const { return m_active; }

	// Ejes locales de la entidad
	inline glm::dvec3 right() { return modelMat[0]; }
	inline glm::dvec3 up() { return modelMat[1]; }
	inline glm::dvec3 forward() { return modelMat[2]; }

	/* Devuelve la lista de componentes de la entidad */
	inline std::vector<Component*> getComponents() { return m_componentes; }

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

	/* Material usado por la entidad */
	Material m_material;

	/* Manda la información de la entidad al shader */
	void sendUniforms(Shader* sh);
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
	Cubo(GLdouble size, bool equalFaces = true);
	~Cubo() { };
};

// - - - - - - - - - - - - 

class Esfera : public Entity
{
public:
	Esfera(GLdouble radio, GLuint paralelos = 10, GLuint meridianos = 20);
	~Esfera() { };
};

// - - - - - - - - - - - - 

class Cilindro : public Entity
{
public:
	Cilindro(GLdouble radio, GLdouble altura, GLuint lados = 20);
	~Cilindro() { };
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
	~VBOEntity() { delete vbo; };

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
	~EBOEntity() { delete ebo; delete vbo; };

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
	~VAOEntity() { delete vao; };

	void render() override;

private:
	GLenum type;
	unsigned int numVerts;
	unsigned int numIndices;

	VertexArray* vao;
};

#endif
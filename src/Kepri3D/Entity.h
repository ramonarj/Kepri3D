#ifndef __ENTITY__
#define __ENTITY__

class Mesh;
class Texture;
class Shader;
class Component;

#include "Material.h"

#include <glm.hpp>

#include <iostream>
#include "Utils.h"

const unsigned int NUM_TEXTURES = 5;

class Entity
{
public:
	/* Constructora por defecto */
	Entity();
	Entity(const std::string& name);
	
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

	/* Establece la textura secundaria a usar, que se mezclará con la principal en la medida dada */
	void setSecondTex(const std::string& textureID);

	/* Establece el mapa especular (textura en B/N) que se usará para 'reemplazar' la componente specular del material */
	void setSpecularMap(const std::string& textureID);

	/* Establece el mapa de normales (bump map) que se usará para 'reemplazar' las normales de la malla */
	void setNormalMap(const std::string& textureID);

	/* Establece el mapa de desplazamiento (textura en B/N) que desplazará las coordenadas de textura de la malla */
	void setDisplacementMap(const std::string& textureID);

	/* Cómo debe pintarse la entidad por delante y detrás (GL_FILL/GL_LINE/GL_POINT) */
	void setPolygonMode(GLenum front, GLenum back);

	// Getters
	/* Devuelve el nombre de la entidad */
	const std::string& getName() const { return m_name; }

	/* Devuelve la posición de la entidad */
	glm::dvec3 getPosition() const { return modelMat[3]; }

	/* Devuelve la matriz de modelado de la entidad */
	const glm::dmat4& getModelMat() const { return modelMat; }

	/* Devuelve el material que usa la entidad */
	const Mesh* getMesh() const { return m_mesh; }

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

	/* Hijos de la entidad */
	std::vector<Entity*> m_children;

	/* Padre de la entidad */
	Entity* m_parent;

	/* Malla/s que usará la entidad para pintarse */
	Mesh* m_mesh;

	/* Matriz de modelado de la entidad; recoge las transformaciones 
	(traslaciones, rotaciones, escalados) hechas a la entidad */
	glm::dmat4 modelMat;

	/* Texturas utilizada por la entidad */
	// 0 = Textura Principal
	// 1 = Textura Secundaria
	// 2 = Specular Map
	// 3 = Normal Map
	// 4 = Displacement Map
	Texture* m_textures[NUM_TEXTURES];

	/* Material usado por la entidad */
	Material m_material;

	/* Shader usado por la entidad */
	Shader* m_shader;

	/* Forma en que debe pintarse */
	GLenum m_polyModeFront;
	GLenum m_polyModeBack;

	// Métodos auxiliares
	/* Manda las texturas al shader */
	void bindTextures();
	/* Limpia los atributos */
	void unbindTextures();
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

class MovingGrid : public Grid
{
public:
	MovingGrid(GLuint filas, GLuint columnas, GLdouble tamFila, GLdouble tamColumna);
	~MovingGrid() { };
	void render() override;

	void setSpeeds(glm::vec2 texSpeed, glm::vec2 dispSpeed) { velTex = texSpeed; velDisp = dispSpeed; }
private:
	glm::vec2 velTex;
	// Útil para simular la dirección y fuerza del viento en la superficie del agua
	glm::vec2 velDisp;
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

class CuboMultitex : public Entity
{
public:
	CuboMultitex(GLdouble size);
	~CuboMultitex() { };

	void render() override;
};

// - - - - - - - - - - - - 

class CuboSpecmap : public Entity
{
public:
	CuboSpecmap(GLdouble size);
	~CuboSpecmap() { };
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

class NormalMapWall : public Entity
{
public:
	NormalMapWall(GLuint filas, GLuint columnas, GLdouble tamFila, GLdouble tamColumna);
	~NormalMapWall() { };
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
class Camera;

class TessTerrain : public Entity
{
public:
	/* Crea un terreno vacío */
	TessTerrain(GLuint filas, GLuint columnas, GLdouble tamFila, GLdouble tamColumna);
	~TessTerrain() { };

	/* Crea el terreno con un mapa de alturas BMP (tendrá tantos vértices como píxeles la imagen) */
	//void loadHeightMap(const std::string& heightFile, GLdouble scale = 1.0);

	inline void setCamera(Camera* cam) { this->cam = cam; }
	void setHeightMap(const std::string& hMap, float elevacion);

	void render() override;

	bool useEyedir;
private:
	Camera* cam;
	float elevacion;
	int patchSize;
};


#endif
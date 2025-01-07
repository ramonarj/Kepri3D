#ifndef __SCENE__
#define __SCENE__

#include <vector>
#include <glew.h>
#include <glm.hpp>

class Entity;
class Camera;
class Light;
class Canvas;
class Shader;
class Mesh;
class Skybox;

// Clase abstracta que representa una escena de juego, con sus entidades, luces, etc.
class Scene
{
public:
	/* Constructora por defecto*/
	Scene(){}
	Scene(Camera* cam) : m_camera(cam), m_canvas(nullptr), m_effectsMesh(nullptr), m_skybox(nullptr) { };

	/* Destructora */
	~Scene();

	/* Inicia los subsistemas de openGL y crea texturas y entidades */
	virtual void init() = 0;
	/* Pinta todas las entidades */
	void render();
	/* Actualiza todas las entidades */
	void update(GLuint deltaTime);

	/* Añade una entidad a la escena */
	void AddEntity(Entity* e, bool isTranslucid = false);

	/* Añade una luz a la escena */
	inline void AddLight(Light* l) { m_lights.push_back(l); }

	/* Hace una foto */
	void takePhoto();

protected:
	/* Lista de entidades */
	std::vector<Entity*> m_entities;

	/* Lista de luces */
	std::vector<Light*> m_lights;

	/* Cámara activa */
	Camera* m_camera;

	/* Skybox */
	Skybox* m_skybox;

	/* Canvas de la escena */
	Canvas* m_canvas;

	/* Efecto de postprocesado que se aplicará a la escena */
	static Shader* compositeShader;

	/* Malla para pintar los efectos */
	Mesh* m_effectsMesh;


	// Extra
	static Shader* normalsShader;
	static bool skyboxActive;
	static bool mipmapsActive;

	// Métodos auxiliares

	static void switchBoolParam(GLenum param);


	// Callbacks para los botones
	static void cullingButtonPressed();
	static void blendingButtonPressed();
	static void lightingButtonPressed();
	static void texturesButtonPressed();
	static void shadingButtonPressed();
	static void alphaButtonPressed();
	static void multisamplingButtonPressed();
	static void mipmapButtonPressed();
	static void normalsButtonPressed();
	static void compositeButtonPressed();
	static void scissorButtonPressed();
	static void skyboxButtonPressed();
	static void gammaButtonPressed();

private:
	// Sub-métodos del render() para que sea más legible
	void loadLights();
	void renderSkybox(const glm::dmat4& projViewMat);
	void renderEntities(const glm::dmat4& projViewMat);
	void renderNormals(const glm::dmat4& projViewMat);
	void renderCanvas();
	void renderEffects();
};

#endif
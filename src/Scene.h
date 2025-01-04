#ifndef __SCENE__
#define __SCENE__

#include <vector>
#include <glew.h>

class Entity;
class Camera;
class Light;
class Canvas;
class Shader;
class Mesh;

class Scene
{
public:
	/* Constructora por defecto*/
	Scene(Camera* cam) : m_camera(cam), m_canvas(nullptr) { };
	~Scene();

	/* Inicia los subsistemas de openGL y crea texturas y entidades */
	void init();
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

	/* Canvas de la escena */
	Canvas* m_canvas;

	/* Malla para pintar los efectos */
	Mesh* m_effectsMesh;

	static Shader* normalsShader;
	static Shader* compositeShader;
	static bool shadersActive;
	static bool mipmapsActive;

	// Métodos auxiliares

	void ViewportTest();

	void PruebaMateriales();

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
	static void shaderButtonPressed();
};

#endif
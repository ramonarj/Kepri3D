#ifndef __SCENE__
#define __SCENE__

#include <vector>
#include <freeglut.h>

class Entity;
class Camera;
class Light;
class Canvas;

class Scene
{
public:
	/* Constructora por defecto*/
	Scene(Camera* cam) : m_camera(cam) { };
	~Scene();

	/* Inicia los subsistemas de openGL y crea texturas y entidades */
	void init();
	/* Pinta todas las entidades */
	void render();
	/* Actualiza todas las entidades */
	void update(GLuint deltaTime);

	/* Devuelve la luz puntual */
	Light* getLight() { return m_lights[0]; }

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

	// Métodos auxiliares
	void initGLSubsystems();

	void ViewportTest();

	void PruebaMateriales();
	
	static void buttonPressed();

	//Camera* camera;
	//Foto* foto;
	//Light* dirLight, * spotLight, * sphereLight;
};

#endif
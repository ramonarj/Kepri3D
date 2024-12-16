#ifndef __SCENE__
#define __SCENE__

#include <vector>
#include <freeglut.h>

class Texture;
class Entity;
class Camera;
class Light;

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

protected:
	/* Lista de texturas */
	std::vector<Texture*> m_textures;
	/* Lista de entidades */
	std::vector<Entity*> m_entities;
	/* Lista de luces */
	std::vector<Light*> m_lights;

	/* Cámara activa */
	Camera* m_camera;

	// Métodos auxiliares
	void initGLSubsystems();

	void ViewportTest();

	//Camera* camera;
	//Foto* foto;
	//Light* dirLight, * spotLight, * sphereLight;
};

#endif
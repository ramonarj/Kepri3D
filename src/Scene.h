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
	Scene(Camera* cam) : m_camera(cam), m_pointLight(nullptr), m_dirLight(nullptr) { };
	~Scene();

	/* Inicia los subsistemas de openGL y crea texturas y entidades */
	void init();
	/* Pinta todas las entidades */
	void render();
	/* Actualiza todas las entidades */
	void update(GLuint deltaTime);

protected:
	/* Lista de texturas */
	std::vector<Texture*> m_textures;
	/* Lista de entidades */
	std::vector<Entity*> m_entities;

	/* Cámara activa */
	Camera* m_camera;

	/* Luz puntual */
	Light* m_pointLight;

	/* Luz direccional */
	Light* m_dirLight;

	// Métodos auxiliares
	void initGLSubsystems();

	void ViewportTest();

	//Camera* camera;
	//Foto* foto;
	//Light* dirLight, * spotLight, * sphereLight;
};

#endif
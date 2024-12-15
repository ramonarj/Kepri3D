#ifndef __SCENE__
#define __SCENE__

#include <vector>
#include <freeglut.h>

class Texture;
class Entity;
class Camera;

class Scene
{
public:
	Scene(Camera* cam) : m_camera(cam) { };
	//Scene(Camera* cam) : camera(cam) { };
	~Scene();
	void init();
	void render();
	void update(GLuint timeElapsed);
	//Light* getDirLight() { return dirLight; };
	//Light* getSpotLight() { return spotLight; };
	//Light* getSphereLight() { return sphereLight; };

protected:
	/* Lista de texturas */
	std::vector<Texture*> m_textures;
	/* Lista de entidades */
	std::vector<Entity*> m_entities;

	/* Cámara activa */
	Camera* m_camera;

	// Métodos auxiliares
	void ViewportTest();

	//Camera* camera;
	//Foto* foto;
	//Light* dirLight, * spotLight, * sphereLight;
};

#endif
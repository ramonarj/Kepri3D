#ifndef __SCENE__
#define __SCENE__

#include "Entity.h"
#include <vector>
#include "Camera.h"

class Scene
{
public:
	Scene(Camera* cam) : m_camera(cam) { };
	//Scene(Camera* cam) : camera(cam) { };
	~Scene();
	void init();
	void render();
	//void update(GLuint timeElapsed);
	//Light* getDirLight() { return dirLight; };
	//Light* getSpotLight() { return spotLight; };
	//Light* getSphereLight() { return sphereLight; };

protected:
	/* Lista de entidades */
	std::vector<Entity*> m_entities;

	/* Cámara activa */
	Camera* m_camera;

	//Camera* camera;
	//Diabolo* diabolo;
	//Foto* foto;
	//Light* dirLight, * spotLight, * sphereLight;
	//std::vector<Entity*> objetos;
};

#endif
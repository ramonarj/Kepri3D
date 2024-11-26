#ifndef __SCENE__
#define __SCENE__

#include <glm.hpp>

class Scene
{
public:
	Scene() { };
	//Scene(Camera* cam) : camera(cam) { };
	~Scene();
	void init();
	void render();
	//void update(GLuint timeElapsed);
	//Light* getDirLight() { return dirLight; };
	//Light* getSpotLight() { return spotLight; };
	//Light* getSphereLight() { return sphereLight; };

protected:
	/* Número total de vértices */
	int numVertices;
	/* Array de vértices */
	glm::dvec3* vertices;
	/* Color de cada uno de los vértices */
	glm::dvec3* colores;
	//Camera* camera;
	//Diabolo* diabolo;
	//Foto* foto;
	//Light* dirLight, * spotLight, * sphereLight;
	//std::vector<Entity*> objetos;
};

#endif
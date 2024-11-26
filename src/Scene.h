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
	/* N�mero total de v�rtices */
	int numVertices;
	/* Array de v�rtices */
	glm::dvec3* vertices;
	/* Color de cada uno de los v�rtices */
	glm::dvec3* colores;
	//Camera* camera;
	//Diabolo* diabolo;
	//Foto* foto;
	//Light* dirLight, * spotLight, * sphereLight;
	//std::vector<Entity*> objetos;
};

#endif
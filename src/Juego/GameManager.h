#ifndef __GAMEMANAGER__
#define __GAMEMANAGER__

#include "../Entity.h"

class Camera;
class Scene;
class Light;
class UIElement;

class GameManager : public Entity
{
public:
	GameManager(Scene* scene, Camera* cam, UIElement* botonesMenu, Entity* particleSystem) :
		dirLight(nullptr), circleLight(nullptr), spotLight(nullptr)
	{ 
		this->scene = scene;
		this->cam = cam; 
		this->botonesMenu = botonesMenu;
		this->pSystem = particleSystem;
	}
	~GameManager(){}

	void setLights(Light* dirLight, Light* circleLight, Light* spotLight);

	void update(GLuint deltaTime) override;
private:
	Camera* cam;
	Scene* scene;
	Light* dirLight, *circleLight, *spotLight;
	Entity* pSystem;
	UIElement* botonesMenu;

	glm::vec3 movCamara = { 0,0,0 };
	double velCamara = 8.0f;
	bool lockedMouse = true;
	bool fullscreen = false;
	GLdouble velTorre = 10.0f;
	GLuint totalTime = 0;

	// Métodos
	void movimientoCamara(GLuint deltaTime);
	void rotacionesCamara(GLuint deltaTime);
	void volumenVistaCamara(GLuint deltaTime);


	void controlLuces(GLuint deltaTime);
	void controlTorre(GLuint deltaTime);
};

#endif
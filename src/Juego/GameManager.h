#ifndef __GAMEMANAGER__
#define __GAMEMANAGER__

#include "../Component.h"

class Camera;
class Scene;
class Light;
class UIElement;

class GameManager : public Component
{
public:
	GameManager(Scene* scene, Camera* cam, UIElement* botonesMenu, Entity* particleSystem) :
		dirLight(nullptr), circleLight(nullptr), spotLight(nullptr)
	{ 
		this->scene = scene;
		this->cam = cam; 
		this->botonesMenu = botonesMenu;
		this->pSystem = particleSystem;

		//this->m_name = "GameManager";
	}
	~GameManager(){}

	void setLights(Light* dirLight, Light* circleLight, Light* spotLight);

	void update(GLuint deltaTime) override;
private:
	// Referencias a los GameObjects
	Camera* cam;
	Scene* scene;
	Light* dirLight, *circleLight, *spotLight;
	Entity* pSystem;
	UIElement* botonesMenu;

	// Cursor y pantalla
	bool lockedMouse = true;
	bool fullscreen = false;
	// Luces
	GLuint totalTime = 0;
	float sunVel = 0.1;
	bool movingLights = true;
	// Partículas
	GLdouble velTorre = 10.0f;

	// Métodos
	void controlLuces(GLuint deltaTime);
	void controlTorre(GLuint deltaTime);
};

#endif
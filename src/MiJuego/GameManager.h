#ifndef __GAMEMANAGER__
#define __GAMEMANAGER__

#include "../Component.h"

class Camera;
class Scene;
class Light;
class UIElement;
class ParticleSystem;
class Terreno;

class GameManager : public Component
{
public:
	GameManager(Scene* scene, Camera* cam, UIElement* botonesMenu, Entity* particleSystem) :
		dirLight(nullptr), circleLight(nullptr), spotLight(nullptr), luzBlinn(nullptr), tessTerrain(nullptr)
	{
		this->scene = scene;
		this->cam = cam;
		this->botonesMenu = botonesMenu;
		this->pSystem = particleSystem;

		//this->m_name = "GameManager";
	}
	~GameManager(){}

	void setLights(Light* dirLight, Light* circleLight, Light* spotLight, Light* luzBlinn);
	void setTessTerrain(Terreno* tesTerrain) { this->tessTerrain = tesTerrain; }
	void setParticleSys(ParticleSystem* partSys) { this->particleSys = partSys; }

	void update(GLuint deltaTime) override;

	static void centerMouse();

	// Sistema de partículas
	static ParticleSystem* particleSys;

private:
	// Referencias a los GameObjects
	Camera* cam;
	Scene* scene;
	Light* dirLight, *circleLight, *spotLight, *luzBlinn;
	Entity* pSystem;
	UIElement* botonesMenu;
	Terreno* tessTerrain;

	// Cursor y pantalla
	bool lockedMouse = true;
	bool fullscreen = false;
	glm::ivec2 windowDim;
	// Luces
	GLuint totalTime = 0;
	float sunVel = 0.15;
	float lightVel = 0.5f;
	bool movingLights = true;
	// Partículas
	GLdouble velTorre = 10.0f;

	// Métodos
	void controlLuces(GLuint deltaTime);
	void controlTorre(GLuint deltaTime);
	void controlTerreno(GLuint deltaTime);
};

#endif
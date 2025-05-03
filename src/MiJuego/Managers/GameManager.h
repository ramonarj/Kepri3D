#ifndef __GAMEMANAGER__
#define __GAMEMANAGER__

#include "../Component.h"

class Camera;
class Scene;
class Light;
class UIElement;
class ParticleSystem;
class Terreno;
class AnimationClip;

class GameManager : public Component
{
public:
	GameManager(Scene* scene, Camera* cam, UIElement* botonesMenu, ParticleSystem* particleSystem) :
		circleLight(nullptr), spotLight(nullptr), luzBlinn(nullptr), tessTerrain(nullptr)
	{
		this->scene = scene;
		this->cam = cam;
		this->botonesMenu = botonesMenu;
		this->pSystem = particleSystem;

		//this->m_name = "GameManager";
	}
	~GameManager(){}

	void setLights(Light* circleLight, Light* spotLight, Light* luzBlinn);
	void setTessTerrain(Terreno* tesTerrain) { this->tessTerrain = tesTerrain; }
	void setParticleSys(ParticleSystem* partSys) { this->pSystem = partSys; }

	void start() override;
	void update(float deltaTime) override;

	static void centerMouse();

private:
	// Referencias a los GameObjects
	Camera* cam;
	Scene* scene;
	Light *circleLight, *spotLight, *luzBlinn;
	ParticleSystem* pSystem;
	UIElement* botonesMenu;
	Terreno* tessTerrain;
	AnimationClip* animClip;

	// Cursor y pantalla
	bool lockedMouse = true;
	bool fullscreen = false;
	glm::ivec2 windowDim;
	// Luces
	float totalTime = 0;
	float lightVel = 0.5f;
	bool movingLights = true;
	// Partículas
	GLdouble velTorre = 10.0f;

	// Métodos
	void controlLuces(float deltaTime);
	void controlMovimiento(Entity* e, float deltaTime);
	void controlTerreno(float deltaTime);
};

#endif
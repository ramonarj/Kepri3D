#ifndef __GAMEMANAGER__
#define __GAMEMANAGER__

#include "Entity.h"

class Camera;
class Scene;

class GameManager : public Entity
{
public:
	GameManager(Scene* scene, Camera* cam) { this->scene = scene; this->cam = cam; }
	~GameManager(){}

	void update(GLuint deltaTime) override;
private:
	Camera* cam;
	Scene* scene;

	glm::vec3 movCamara = { 0,0,0 };
	double velCamara = 0.4;
	bool lockedMouse = true;
	bool fullscreen = false;
	float velLuz = 0.02f;

	// Métodos
	void movimientoCamara();
	void rotacionesCamara();

	void controlLuces();
};

#endif
#ifndef __CAMERA_CONTROLLER__
#define __CAMERA_CONTROLLER__

#include "Component.h"

class Camera;

class CameraController : public Component
{
public:
	CameraController(Camera* cam);
	~CameraController(){}

	void update(GLuint deltaTime) override;
private:
	Camera* cam;

	double velocidad = 16.0f;
	float sensibilidad = 1.5;

	// Métodos
	void movimientoCamara(GLuint deltaTime);
	void rotacionesCamara(GLuint deltaTime);
	void volumenVistaCamara(GLuint deltaTime);
};
#endif

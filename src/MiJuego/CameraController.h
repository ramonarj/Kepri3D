#ifndef __CAMERA_CONTROLLER__
#define __CAMERA_CONTROLLER__

#include "Component.h"

class Camera;

class CameraController : public Component
{
public:
	CameraController(Camera* cam);
	~CameraController(){}

	void update(float deltaTime) override;
private:
	Camera* cam;

	double velocidad = 40.0f;
	float sensibilidad = 0.25;

	// Métodos
	void movimientoCamara(float deltaTime);
	void rotacionesCamara(float deltaTime);
	void volumenVistaCamara(float deltaTime);
	void puntosFijosCamara();
};
#endif

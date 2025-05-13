#ifndef __THIRD_PERSON_CAMERA__
#define __THIRD_PERSON_CAMERA__

#include "Component.h"

class Camera;

class TPCamera : public Component
{
public:
	TPCamera(Entity* target);
	~TPCamera(){}

	void start() override;
	void update(float deltaTime) override;
	void fixedUpdate(float fixedTime) override;
private:
	Camera* cam;
	Entity* target;
	Entity* cabeza;

	double distancia;

	// Métodos
	void movimiento(float deltaTime);
	void rotaciones(float deltaTime);
};
#endif

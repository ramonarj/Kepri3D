#ifndef __ROTATION_COMP__
#define __ROTATION_COMP__

#include "Component.h"

class RotationComp : public Component
{
public:
	RotationComp(GLdouble rotSpeed = 1.0) : rotSpeed(rotSpeed), rotAxis({ 0, 1, 0 }) {}
	~RotationComp(){}

	void update(float deltaTime) override;

	void onCollision(Collider* other);

private:
	GLdouble rotSpeed;
	glm::vec3 rotAxis;
};

#endif

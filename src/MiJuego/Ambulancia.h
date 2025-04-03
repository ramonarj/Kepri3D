#ifndef __AMBULANCIA_COMP__
#define __AMBULANCIA_COMP__

#include "Component.h"

class Audio;
class Entity;

class Ambulancia : public Component
{
public:
	Ambulancia();
	~Ambulancia() {}

	void update(float deltaTime) override;
private:
	float vel = 80.0f;
	unsigned char estado;
};

#endif

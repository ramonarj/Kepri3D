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
	float vel = 27.0f; //Importante: esto está en metros/segundo!!
	unsigned char estado;
};

#endif

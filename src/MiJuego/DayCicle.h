#ifndef __DAY_CICLE__
#define __DAY_CICLE__

#include "Component.h"

class Light;
class Skybox;

class DayCicle : public Component
{
public:
	/* Número de segundos que durará un 'día', desde un amanecer hasta el siguiente */
	DayCicle(int dayDuration = 100);
	DayCicle(Light* dirLight, Skybox* cielo);
	~DayCicle(){}

	void start() override;
	void update(float deltaTime) override;
private:
	Light* sol, * luna;
	Skybox* cielo;
	bool day = true;
	float velSol = 0.15;
	float totalTime = 0;
	bool running = true;
};

#endif
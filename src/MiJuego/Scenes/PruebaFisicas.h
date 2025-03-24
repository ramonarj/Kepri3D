#ifndef __FISICAS_SCENE__
#define __FISICAS_SCENE__

#include "../Scene.h"

class Rigid;

class PruebaFisicas : public Scene
{
public:
	PruebaFisicas() { m_name = "PruebaFisicas"; }
	~PruebaFisicas() {}

	void loadResources() override;
	void init() override;

private:
	void createDragon(Rigid* nodo, int numEslabones, int tipoUnion);
};

#endif
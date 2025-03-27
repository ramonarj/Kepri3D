#ifndef __SONIDO_SCENE__
#define __SONIDO_SCENE__

#include "../Scene.h"

class PruebaSonido : public Scene
{
public:
	PruebaSonido() { m_name = "PruebaSonido"; }
	~PruebaSonido() {}

	void loadResources() override;
	void init() override;
};

#endif
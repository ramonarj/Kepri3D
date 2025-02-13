#ifndef __PRUEBA_SCENE__
#define __PRUEBA_SCENE__

#include "../Scene.h"

class ParticleSystem;

class PruebaScene : public Scene
{
public:
	PruebaScene() { m_name = "Prueba"; }
	~PruebaScene() {}

	// M�todos a implementar de 'Scene'
	void loadResources() override;
	void init() override;

private:
	// Vector de callbacks
	static std::vector<void(*)()> callbacks;

	// M�todos auxiliares
	void ViewportTest();
	void PruebaMateriales();
};

#endif
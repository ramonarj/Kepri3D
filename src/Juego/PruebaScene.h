#ifndef __PRUEBA_SCENE__
#define __PRUEBA_SCENE__

#include "../Scene.h"

class PruebaScene : public Scene
{
public:
	PruebaScene(Camera* cam) { this->m_camera = cam; }
	~PruebaScene() {}

	void init() override;

private:
	// Vector de callbacks
	static std::vector<void(*)()> callbacks;

	// Métodos auxiliares
	void ViewportTest();
	void PruebaMateriales();
};

#endif
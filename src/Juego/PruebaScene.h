#ifndef __PRUEBA_SCENE__
#define __PRUEBA_SCENE__

#include "../Scene.h"

class ParticleSystem;

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
	void loadResources();
	void ViewportTest();
	void PruebaMateriales();

	// Sistema de partículas
	static ParticleSystem* particleSys;

	static void centerMouse();

	// Callbacks para los botones
	static void cullingButtonPressed();
	static void blendingButtonPressed();
	static void lightingButtonPressed();
	static void texturesButtonPressed();
	static void shadingButtonPressed();
	static void alphaButtonPressed();
	static void multisamplingButtonPressed();
	static void mipmapButtonPressed();
	static void normalsButtonPressed();
	static void compositeButtonPressed();
	static void scissorButtonPressed();
	static void skyboxButtonPressed();
	static void gammaButtonPressed();
	static void stencilButtonPressed();
	static void logicOpButtonPressed();
	static void instancingButtonPressed();
};

#endif
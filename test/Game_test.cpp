#include "Game_test.h"

#include "Game.h"
#include "Graficos/Camera.h"

void GameSuite::setup()
{
	addTest(test_addViewport);
	addTest(test_getViewport);
	addTest(test_getCamera);
	addTest(test_getScene);
	addTest(test_init);
}

void GameSuite::test_init()
{
	Game* g = Game::Instance();

	// Caso normal
	int w = 800; int h = 600;
	g->init(s_argc, s_argv, w, h, "MockWindow");

	assert(g->getWindowSize().x == w);
	assert(g->getWindowSize().y == h);
	assert(g->getViewport()->getW() == w);
	assert(g->getViewport()->getH() == h);


	// Pantalla completa
	//g = Game::Instance();


	g->clean();
}

void GameSuite::test_addViewport()
{
	Game* g = Game::Instance();

	// Caso normal
	Viewport* vp = g->addViewport(200, 200, 0, 0);
	assert(vp != nullptr);
	assert(vp == g->getViewport(0));

	// Hay que hacer esto porque es un Singleton, y el estado se conservaría entre tests
	g->clean();
}

void GameSuite::test_getViewport()
{
	Game* g = Game::Instance();

	// No hay vieports todavía
	assert(g->getViewport(0) == nullptr);
	assert(g->getViewport(40) == nullptr);
	assert(g->getViewport(-1) == nullptr);

	// Caso normal
	Viewport* vp1 = g->addViewport(200, 200, 0, 0);
	Viewport* vp2 = g->addViewport(200, 200, 0, 0);
	assert(vp1 == g->getViewport(0));
	assert(vp2 == g->getViewport(1));
	assert(vp1 != vp2);

	g->clean();
}

void GameSuite::test_getCamera()
{
	Game* g = Game::Instance();

	// No hay cámaras todavía
	assert(g->getCamera(-1) == nullptr);
	assert(g->getCamera(0) == nullptr);
	assert(g->getCamera(1) == nullptr);

	g->clean();
}

void GameSuite::test_getScene()
{
	Game* g = Game::Instance();

	// No hay escena todavía
	assert(g->getScene() == nullptr);

	// Caso normal. TODO
	MockScene* mockScene = new MockScene();
	//g->loadScene(mockScene);
	//assert(g->getScene() == mockScene);

	g->clean();
}
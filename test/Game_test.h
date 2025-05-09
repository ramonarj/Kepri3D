#ifndef __GAME_SUITE__
#define __GAME_SUITE__

#include "TestSuite.h"
#include "Scene.h"

class GameSuite : public TestSuite
{
public:
	void setup() override;
protected:
	// Tests
	Test test_init();

	Test test_addViewport();
	Test test_getViewport();
	Test test_getCamera();
	Test test_getScene();
};

class MockScene : public Scene
{
	void loadResources() override {}
	void setup() override {};
};

#endif
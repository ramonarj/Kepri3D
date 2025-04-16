#ifndef __SANDBOX_SCENE__
#define __SANDBOX_SCENE__

#include "Scene.h"

class Sandbox : public Scene
{
public:
	Sandbox() { m_name = "Sandbox"; }
	~Sandbox() {}

	void loadResources() override;
	void setup() override;

private:

};

#endif
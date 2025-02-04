#ifndef __VUELO_SCENE__
#define __VUELO_SCENE__

#include "../Scene.h"

class SimVuelo : public Scene
{
public:
	SimVuelo() { m_name = "SimVuelo"; }
	~SimVuelo() {}

	void loadResources() override;
	void init() override;
};

#endif
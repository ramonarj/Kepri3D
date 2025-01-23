#ifndef __SPACE_SCENE__
#define __SPACE_SCENE__

#include "../Scene.h"

class SpaceScene : public Scene
{
public:
	SpaceScene() { m_name = "Espacio"; }
	~SpaceScene() {}

	void init() override;
private:
};

#endif
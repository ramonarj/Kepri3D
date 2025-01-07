#ifndef __SPACE_SCENE__
#define __SPACE_SCENE__

#include "../Scene.h"

class SpaceScene : public Scene
{
public:
	SpaceScene(Camera* cam) : Scene(cam){}
	~SpaceScene() {}

	void init() override;
private:
};

#endif
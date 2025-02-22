#include "SpaceScene.h"

#include "../Kepri3D.h"

void SpaceScene::loadResources()
{
	ResourceManager::Instance()->loadTexture("venus.bmp", "venus");
}

void SpaceScene::init()
{
	Camera::setBackgroundColor(0, 0, 0);

	// Luz
	Light* dirLight = new Light(DIRECTIONAL_LIGHT);
	dirLight->setDirection({ 0,-1 , 0 });
	dirLight->setAmbient({0.6, 0, 0, 0});
	//AddLight(dirLight);

	// Venus
	Esfera* venus = new Esfera(3, 20, true);
	venus->setTexture("venus");
	venus->setPosition({ 0,5,0 });
	AddEntity(venus);
}
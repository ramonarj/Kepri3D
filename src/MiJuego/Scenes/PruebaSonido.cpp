#include "PruebaSonido.h"

#include "../Kepri3D.h"

#include "../AudioMan.h"

void PruebaSonido::loadResources()
{
	ResourceManager::Instance()->loadTexture("venus.bmp", "venus");

	ResourceManager::Instance()->loadCubemapTexture({ "skyboxes/city/right.jpg", "skyboxes/city/left.jpg", "skyboxes/city/bottom.jpg",
		"skyboxes/city/top.jpg", "skyboxes/city/front.jpg", "skyboxes/city/back.jpg" }, "citySkybox");

	// - - - AUDIO - - - //
	ResourceManager::Instance()->loadAudio("judia.wav", "judia");
	ResourceManager::Instance()->loadAudio("concha.wav", "concha");
}

void PruebaSonido::init()
{
	Camera::setBackgroundColor(0, 0, 0);

	// Luz
	Light* dirLight = new Light(DIRECTIONAL_LIGHT);
	dirLight->setDirection({ 0,-1 , 0 });
	dirLight->setDiffuse({ 1, 1, 1, 1.0 });
	dirLight->setAmbient({ 0.05, 0.05, 0.2, 0.0 });
	Entity* dirLightEnt = new Entity({ dirLight });
	AddEntity(dirLightEnt);

	// Venus
	Esfera* venus = new Esfera(3, 20, 40);
	venus->setMaterial("default");
	venus->setTexture("venus");
	venus->setPosition({ 0,10,0 });
	AddEntity(venus);

	// Audio Source
	AudioSource* audioComp = new AudioSource("judia");
	venus->addComponent(audioComp);

	// AudioManager
	Audio* fxConcha = (Audio*)&ResourceManager::Instance()->getAudio("concha");
	Entity* audioMan = new Entity("AudioMan");
	audioMan->addComponent(new AudioMan(audioComp, fxConcha));
	AddEntity(audioMan);

	/* - - Skybox - - */
	Skybox* sky = new Skybox("citySkybox");
	SetSkybox(sky);


	// Temporal
	for (Entity* e : m_entities)
	{
		if (e->getRenderer() != nullptr)
		{
			Shader* sh = (Shader*)e->getShader();
			if (sh != nullptr)
			{
				sh->bindUniformBlock("Matrices", 0);
				sh->bindUniformBlock("Lights", 1);
			}
		}
	}
}
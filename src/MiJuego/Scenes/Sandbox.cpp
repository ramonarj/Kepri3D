#include "Sandbox.h"

#include "Kepri3D.h"

#include "CameraController.h"
#include "DebugText.h"
#include "SandboxMan.h"

void Sandbox::setup()
{
	Camera::setBackgroundColor(0, 0, 0);

	// Luz
	Light* dirLight = new Light(DIRECTIONAL_LIGHT);
	dirLight->setDirection({ 0,-1 , 0 });
	dirLight->setDiffuse({ 1, 1, 1 });
	dirLight->setAmbient({ 0.05, 0.05, 0.2 });
	Entity* dirLightEnt = new Entity({ dirLight });
	AddEntity(dirLightEnt);

	// Suelo
	Grid* grid = new Grid(100, 100, 5, 5);
	grid->setMaterial("blendTerrain");
	grid->getMaterial()->setVec2("tiling", { 20, 20 });
	AddEntity(grid);

	/* - - Skybox - - */
	Skybox* sky = new Skybox("technoSkybox");
	SetSkybox(sky);

	/* - - Canvas - - */
	m_canvas = new Canvas();

	/* - - GameManager - - */
	Entity* gm = new Entity();
	// Componente CameraController
	CameraController* camComp = new CameraController(m_camera);
	gm->addComponent(camComp);
	// Componente SandboxMan
	SandboxManager* sandboxMan = new SandboxManager(grid);
	gm->addComponent(sandboxMan);
	AddEntity(gm);

	// Información de Debug
	Entity* debugTxt = new Entity("DebugText");
	debugTxt->setPosition({ 0,0,0 });
	debugTxt->addComponent(new DebugText(m_canvas, "panel"));
	AddEntity(debugTxt);
	debugTxt->setActive(true);
}


void Sandbox::loadResources()
{
	/* - - Texturas - - */
	ResourceManager::Instance()->loadTexture("UI\\panel.png", "panel");
	ResourceManager::Instance()->loadTexture("cesped.jpg", "cesped");
	ResourceManager::Instance()->loadTexture("tierra.jpg", "tierra");
	ResourceManager::Instance()->loadTexture("nieve.jpg", "nieve");
	ResourceManager::Instance()->loadTexture("agua.jpg", "agua");
	ResourceManager::Instance()->loadTexture("blendMap.png", "blendMap");

	ResourceManager::Instance()->loadCubemapTexture({ "skyboxes/techno/right.png", "skyboxes/techno/left.png", "skyboxes/techno/bottom.png",
		"skyboxes/techno/top.png", "skyboxes/techno/front.png", "skyboxes/techno/back.png" }, "technoSkybox");

	/* - - Shaders - - */
	ResourceManager::Instance()->loadShader("blendMap.vert", "", "blendMap.frag", "blendMap");

	/* - -  Materiales - - */
	ResourceManager::Instance()->loadMaterial("cesped.material", "cesped", "default");
	ResourceManager::Instance()->loadMaterial("blendTerrain.material", "blendTerrain", "blendMap");
}
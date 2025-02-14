#include "PruebaFisicas.h"

#include "../Kepri3D.h"

#include "../CameraController.h"
#include "../PhysicsMan.h"

void PruebaFisicas::init()
{
	// LUCES
	// Direccional
	Light* dirLight = new Light(DIRECTIONAL_LIGHT);
	dirLight->setDirection({ 0, -1 , 0 });
	dirLight->setDiffuse({ 1, 1, 1, 1.0 });
	dirLight->setAmbient({ 0.2, 0.2, 0.2, 0.0 });
	AddLight(dirLight);

	// ENTIDADES

	// Esfera con componente Rigid
	Esfera* esf = new Esfera(1.0);
	esf->setTexture("default");
	esf->setPosition({ 0, 100, 0 });
	Rigid* rigidEsfera = new Rigid(esf->getModelMat());
	rigidEsfera->useGravity(true);
	esf->addComponent(rigidEsfera);
	AddEntity(esf);

	// Suelo sin componente rigid
	Cubo* c = new Cubo(1);
	c->setTexture("cobre");
	c->scale({ 100, 1, 100 });
	AddEntity(c);

	// Sombra de la pelota
	Entity* sombra = new Poligono(40, 3.0, true);
	sombra->setTexture("sombra");
	sombra->rotate(-PI / 2, { 1, 0, 0 });
	AddEntity(sombra);

	/* - - Skybox - - */
	Skybox* sky = new Skybox("lakeSkybox");
	SetSkybox(sky);

	/* - - Canvas - - */
	//m_canvas = new Canvas();
	//m_canvas->setSize(800, 600);

	// PhysicsMan
	Entity* phyMan = new Entity("PhysicsManager");
	PhysicsMan* phyManComp = new PhysicsMan(rigidEsfera, sombra);
	phyMan->addComponent(phyManComp);
	AddEntity(phyMan);

	/* - - GameManager - - */
	Entity* gm = new Entity();
	// Componente CameraController
	CameraController* camComp = new CameraController(m_camera);
	gm->addComponent(camComp);
	AddEntity(gm);


	/* - - - Efectos de postprocesado (el orden importa) - - - */
}

void PruebaFisicas::loadResources()
{
	/* Mallas */

	/* Texturas */
	ResourceManager::Instance()->loadTexture("cobre.bmp", "cobre");
	ResourceManager::Instance()->loadTexture("sombra.png", "sombra");

	ResourceManager::Instance()->loadCubemapTexture({ "skyboxes/right.jpg", "skyboxes/left.jpg", "skyboxes/bottom.jpg",
	"skyboxes/top.jpg", "skyboxes/front.jpg", "skyboxes/back.jpg" }, "lakeSkybox");

	/* Materiales */

	/* Shaders */

	/* Efectos de postprocesado ('composites') */

}

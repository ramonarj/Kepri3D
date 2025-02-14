#include "PruebaFisicas.h"

#include "../Kepri3D.h"

#include "../CameraController.h"

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
	Esfera* esf = new Esfera(5.0);
	esf->setTexture("default");
	esf->setPosition({ 0, 10, 0 });
	Rigid* rigidEsfera = new Rigid(esf->getModelMat());
	rigidEsfera->useGravity(true);
	esf->addComponent(rigidEsfera);
	AddEntity(esf);

	// Suelo sin componente rigid
	Cubo* c = new Cubo(1);
	c->setTexture("earth");
	c->scale({ 20, 1, 20 });
	AddEntity(c);

	/* - - Skybox - - */
	Skybox* sky = new Skybox("lakeSkybox");
	SetSkybox(sky);

	/* - - Canvas - - */
	//m_canvas = new Canvas();
	//m_canvas->setSize(800, 600);

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
	ResourceManager::Instance()->loadTexture("earth24.bmp", "earth");

	ResourceManager::Instance()->loadCubemapTexture({ "skyboxes/right.jpg", "skyboxes/left.jpg", "skyboxes/bottom.jpg",
	"skyboxes/top.jpg", "skyboxes/front.jpg", "skyboxes/back.jpg" }, "lakeSkybox");

	/* Materiales */

	/* Shaders */

	/* Efectos de postprocesado ('composites') */

}

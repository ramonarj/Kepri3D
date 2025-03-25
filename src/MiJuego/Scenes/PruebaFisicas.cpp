#include "PruebaFisicas.h"

#include "../Kepri3D.h"

#include "../RotationComp.h"
#include "../CameraController.h"
#include "../PhysicsMan.h"
#include "../DebugText.h"
#include "../BloqueComp.h"

void PruebaFisicas::init()
{
	m_camera->setPosition({ -100, 10, 20 });
	// LUCES
	// Direccional
	Light* dirLight = new Light(DIRECTIONAL_LIGHT);
	dirLight->setDirection({ 0,-1 , 0 });
	dirLight->setDiffuse({ 1, 1, 1, 1.0 });
	dirLight->setAmbient({ 0.05, 0.05, 0.2, 0.0 });
	Entity* dirLightEnt = new Entity({ dirLight });
	AddEntity(dirLightEnt);

	// ENTIDADES
	m_camera->translate({ 0, -5, 0 });


	// NOTA: hay que añadir el componente Collider antes de añadir el Rigid.
	// Suelo
	Cubo* suelo = new Cubo(1.0);
	suelo->scale({ 200.0, 1.0, 200.0 });
	suelo->setTexture("cobre");
	suelo->setShader("lights");
	suelo->setPosition({ 0, -10, 0 });
	//suelo->getComponent<Renderer>()->setActive(false);
	Collider* colSuelo = new Collider({ 200.0, 1.0, 200.0 });
	colSuelo->setVisible(true);
	suelo->addComponent(colSuelo);
	Rigid* rigidSuelo = new Rigid(suelo->getModelMat(), Static);
	rigidSuelo->useGravity(false);
	rigidSuelo->setMass(100);
	suelo->addComponent(rigidSuelo);
	AddEntity(suelo);

	// Esfera con componente Rigid y Collider visible
	Esfera* esf = new Esfera(1.0);
	esf->setTexture("default");
	esf->setShader("lights");
	esf->setPosition({ 0, 0, 0 });
	esf->getComponent<Renderer>()->setActive(false);
	Collider* colEsfera = new Collider(1.0);
	colEsfera->setVisible(true);
	esf->addComponent(colEsfera);
	Rigid* rigidEsfera = new Rigid(esf->getModelMat(), Static);
	rigidEsfera->useGravity(false);
	//rigidEsfera->setVelocity({ 0, -10, 0 });
	esf->addComponent(rigidEsfera);
	AddEntity(esf);

	// Esfera 2
	Esfera* esf2 = new Esfera(1.0);
	esf2->setTexture("default");
	esf2->setShader("lights");
	esf2->setPosition({ -120, 20, 0 });
	esf2->getComponent<Renderer>()->setActive(false);
	Collider* colEsfera2 = new Collider(5.0);
	colEsfera2->setVisible(true);
	esf2->addComponent(colEsfera2);
	Rigid* rigidEsfera2 = new Rigid(esf2->getModelMat());  // si es estático, no usa la gravedad
	rigidEsfera2->setMass(colEsfera2->volume() * 500); // mitad de densidad que el agua
	//rigidEsfera2->setVelocity({ 0, 10, 0 });
	//rigidEsfera2->useGravity(false);
	esf2->addComponent(rigidEsfera2);
	AddEntity(esf2);

	// Cubo 1
	Cubo* cubo1 = new Cubo(2.0);
	cubo1->setTexture("default");
	cubo1->setShader("lights");
	cubo1->setPosition({ 10, 5, 0 });
	cubo1->getComponent<Renderer>()->setActive(false);
	Collider* colCubo = new Collider({3, 3, 3});
	colCubo->setVisible(true);
	cubo1->addComponent(colCubo);
	Rigid* rigidCubo= new Rigid(cubo1->getModelMat(), Static);
	cubo1->addComponent(rigidCubo);
	AddEntity(cubo1);

	// Cubo 2
	Cubo* cubo2 = new Cubo(2.0);
	cubo2->setTexture("default");
	cubo2->setShader("lights");
	cubo2->setPosition({ 10, 60, 0 });
	cubo2->getComponent<Renderer>()->setActive(false);
	Collider* colCubo2 = new Collider({2.0, 2.0, 2.0});
	colCubo2->setVisible(true);
	cubo2->addComponent(colCubo2);
	Rigid* rigidCubo2 = new Rigid(cubo2->getModelMat());
	rigidCubo2->setMass(1.0);
	rigidCubo2->useGravity(true);
	cubo2->addComponent(rigidCubo2);
	AddEntity(cubo2);

	// Cubo 3
	Cubo* cubo3 = new Cubo(2.0);
	cubo3->setTexture("default");
	cubo3->setShader("lights");
	cubo3->setPosition({ 10, 50, 0 });
	cubo3->getComponent<Renderer>()->setActive(false);
	Collider* colcubo3 = new Collider({2.0, 8.0, 2.0});
	colcubo3->setVisible(true);
	cubo3->addComponent(colcubo3);
	Rigid* rigidcubo3 = new Rigid(cubo3->getModelMat());
	rigidcubo3->setMass(1.0);
	rigidcubo3->useGravity(false);
	cubo3->addComponent(rigidcubo3);
	cubo3->addComponent(new RotationComp(0.0));
	AddEntity(cubo3);


	// Trigger
	Cubo* trigger = new Cubo(0.5);
	trigger->setTexture("default");
	trigger->setShader("lights");
	trigger->setPosition({ 10, 20, 0 });
	trigger->getComponent<Renderer>()->setActive(false);
	Collider* colTrigger = new Collider({ 1, 1, 1});
	colTrigger->setVisible(true);
	colTrigger->setTrigger(true);
	trigger->addComponent(colTrigger);
	Rigid* rigidTrigger = new Rigid(trigger->getModelMat());
	rigidTrigger->useGravity(false);
	trigger->addComponent(rigidTrigger);
	trigger->addComponent(new BloqueComp());
	AddEntity(trigger);

	// Suelo estático
	//Cubo* suelo = new Cubo(1);
	//suelo->setTexture("cobre");
	//suelo->setShader("default");
	//suelo->scale({ 100, 3, 100 });
	////suelo->getComponent<Renderer>()->setActive(false);
	//Collider* colSuelo = new Collider(Collider::Cubo, 1.0);
	////colSuelo->setVisible(true);
	//suelo->addComponent(colSuelo);
	//Rigid* rigidSuelo = new Rigid(suelo->getModelMat());
	//rigidSuelo->useGravity(false);
	//suelo->addComponent(rigidSuelo);
	//AddEntity(suelo);

	// - - - MUELLE - - - //
	Cubo* extremo1 = new Cubo(2.0);
	extremo1->setTexture("default");
	extremo1->setShader("lights");
	extremo1->setPosition({ 0, 0, 15 });
	extremo1->getComponent<Renderer>()->setActive(false);
	Collider* colExtremo1 = new Collider({2.0, 2.0, 2.0});
	colExtremo1->setVisible(true);
	extremo1->addComponent(colExtremo1);
	Rigid* rigidExtremo1 = new Rigid(extremo1->getModelMat(), Static);
	extremo1->addComponent(rigidExtremo1);
	AddEntity(extremo1);

	Cubo* extremo2 = new Cubo(2.0);
	extremo2->setTexture("default");
	extremo2->setShader("lights");
	extremo2->setPosition({ 7.5, 0, 15 });
	extremo2->getComponent<Renderer>()->setActive(false);
	Collider* colExtremo2 = new Collider({2.0, 2.0, 2.0});
	colExtremo2->setVisible(true);
	extremo2->addComponent(colExtremo2);
	Rigid* rigidExtremo2 = new Rigid(extremo2->getModelMat());
	rigidExtremo2->setMass(1.0);
	extremo2->addComponent(rigidExtremo2);
	AddEntity(extremo2);

	// Añadir el muelle
	PhysicsSystem::Instance()->addMuelle(new Muelle(rigidExtremo1, rigidExtremo2, 20));

	// - -  FLOTACIÓN - - //
	// Cubo flotacion
	Cubo* cuboFlot = new Cubo(2.0);
	cuboFlot->setTexture("default");
	cuboFlot->setShader("lights");
	cuboFlot->setPosition({ -110, 10, 20 });
	cuboFlot->getComponent<Renderer>()->setActive(false);
	Collider* colcuboFlot = new Collider({ 2.0, 2.0, 2.0 });
	colcuboFlot->setVisible(true);
	cuboFlot->addComponent(colcuboFlot);
	Rigid* rigidcuboFlot = new Rigid(cuboFlot->getModelMat());
	rigidcuboFlot->setMass(2000.0);
	cuboFlot->addComponent(rigidcuboFlot);
	AddEntity(cuboFlot);

	// Líquido
	Liquido* liquido = new Liquido(-10, 1000);
	PhysicsSystem::Instance()->addLiquido(liquido);

	// - -  ARTICULACIONES - - //
	// Extremo 1
	Cubo* art1 = new Cubo(2.0);
	art1->setTexture("default");
	art1->setShader("lights");
	art1->setPosition({ -40, 40, 0 });
	art1->getComponent<Renderer>()->setActive(false);
	Collider* colArt1 = new Collider({ 2.0, 2.0, 2.0 });
	colArt1->setVisible(true);
	art1->addComponent(colArt1);
	Rigid* rigidArt1 = new Rigid(art1->getModelMat(), Static);
	rigidArt1->setMass(20.0);
	art1->addComponent(rigidArt1);
	AddEntity(art1);

	// Extremo 2
	Cubo* art2 = new Cubo(2.0);
	art2->setTexture("default");
	art2->setShader("lights");
	art2->setPosition({ -45, 40, 0 });
	art2->getComponent<Renderer>()->setActive(false);
	Collider* colArt2 = new Collider({ 2.0, 2.0, 2.0 });
	colArt2->setVisible(true);
	art2->addComponent(colArt2);
	Rigid* rigidArt2 = new Rigid(art2->getModelMat());
	rigidArt2->setMass(20.0);
	art2->addComponent(rigidArt2);
	AddEntity(art2);

	// Añadir la articulación
	//PhysicsSystem::Instance()->addArticulacion(new Articulacion(rigidArt1, rigidArt2, Articulacion::Fija));
	//PhysicsSystem::Instance()->addArticulacion(new Articulacion(rigidArt1, rigidArt2, Articulacion::Bisagra));
	PhysicsSystem::Instance()->addArticulacion(new Articulacion(rigidArt1, rigidArt2, Articulacion::Circular));

	// Dragón
	createDragon(rigidArt2, 8, Articulacion::Circular);

	// Sombra de la pelota
	Entity* sombra = new Poligono(40, 3.0, true);
	sombra->setTexture("sombra");
	sombra->setShader("default");
	sombra->rotate(-PI / 2, { 1, 0, 0 });
	AddEntity(sombra);

	/* - - Skybox - - */
	Skybox* sky = new Skybox("lakeSkybox");
	SetSkybox(sky);

	/* - - Canvas - - */
	m_canvas = new Canvas();
	m_canvas->setSize(800, 600);

	// Información de Debug
	Entity* debugTxt = new Entity("DebugText");
	debugTxt->setPosition({ 0,0,0 });
	debugTxt->addComponent(new DebugText(m_canvas, "panel"));
	AddEntity(debugTxt);
	debugTxt->setActive(true);

	// PhysicsMan
	Entity* phyMan = new Entity("PhysicsManager");
	PhysicsMan* phyManComp = new PhysicsMan(rigidEsfera2, sombra, liquido);
	phyMan->addComponent(phyManComp);
	AddEntity(phyMan);

	/* - - GameManager - - */
	Entity* gm = new Entity();
	// Componente CameraController
	CameraController* camComp = new CameraController(m_camera);
	gm->addComponent(camComp);
	AddEntity(gm);


	/* - - - Efectos de postprocesado (el orden importa) - - - */

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

void PruebaFisicas::loadResources()
{
	/* Mallas */

	/* Texturas */
	ResourceManager::Instance()->loadTexture("cobre.bmp", "cobre");
	ResourceManager::Instance()->loadTexture("sombra.png", "sombra");
	ResourceManager::Instance()->loadTexture("UI\\panel.png", "panel");

	ResourceManager::Instance()->loadCubemapTexture({ "skyboxes/right.jpg", "skyboxes/left.jpg", "skyboxes/bottom.jpg",
	"skyboxes/top.jpg", "skyboxes/front.jpg", "skyboxes/back.jpg" }, "lakeSkybox");

	/* Materiales */

	/* Shaders */

	/* Efectos de postprocesado ('composites') */

}


void PruebaFisicas::createDragon(Rigid* primerNodo, int numEslabones, int tipoUnion)
{
	vector3 pos = primerNodo->getEntity()->getPosition();
	// Dragón
	Rigid* rigidNodo1 = primerNodo;
	Rigid* rigidNodo2;
	for (int i = 0; i < numEslabones; i++)
	{
		// Extremo 2
		Cubo* nodo2 = new Cubo(2.0);
		nodo2->setTexture("default");
		nodo2->setShader("lights");
		nodo2->setPosition(pos - vector3(5, 0, 5));
		nodo2->getComponent<Renderer>()->setActive(false);
		Collider* colNodo2 = new Collider(vector3({ 2.0, 2.0, 2.0 }));// *(1.0 - i * 0.05));
		colNodo2->setVisible(true);
		nodo2->addComponent(colNodo2);
		rigidNodo2 = new Rigid(nodo2->getModelMat());
		rigidNodo2->setMass(20.0);
		nodo2->addComponent(rigidNodo2);
		AddEntity(nodo2);

		// Añadir articulación
		PhysicsSystem::Instance()->addArticulacion(new Articulacion(rigidNodo1, rigidNodo2, (Articulacion::Type)tipoUnion));
		rigidNodo1 = rigidNodo2;
		pos = rigidNodo1->getEntity()->getPosition();
	}
}
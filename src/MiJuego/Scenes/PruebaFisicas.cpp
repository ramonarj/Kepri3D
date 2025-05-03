#include "PruebaFisicas.h"

#include "../Kepri3D.h"

#include "Components/RotationComp.h"
#include "Components/BloqueComp.h"
#include "Managers/CameraController.h"
#include "Managers/GameManager.h"
#include "Managers/PhysicsMan.h"
#include "Managers/DebugText.h"
#include "Generators/Viento.h"

void PruebaFisicas::setup()
{
	//m_camera->setPosition({ -100, 10, 20 });
	// LUCES
	// Direccional
	Light* dirLight = new Light(DIRECTIONAL_LIGHT);
	dirLight->setDirection({ 0,-1 , 0 });
	dirLight->setDiffuse({ 1, 1, 1 });
	dirLight->setAmbient({ 0.05, 0.05, 0.2 });
	Entity* dirLightEnt = new Entity({ dirLight });
	AddEntity(dirLightEnt);

	// ENTIDADES
	m_camera->translate({ 0, -5, 0 });

	// Suelo
	Rigid* suelo = createRigidCubo("Suelo", { 0, -10, 0 }, Static, 100, false, 1.0, { 200, 1, 200 });
	suelo->getEntity()->setTexture("cobre");
	suelo->getEntity()->scale({ 200.0, 1.0, 200.0 });
	suelo->getEntity()->getComponent<Renderer>()->setActive(true);

	// - - ESFERAS - - //
	// Esfera con componente Rigid y Collider visible
	Rigid* esfera1 = createRigidEsfera("Esfera1", { 0, 50, 0 }, Dynamic, 10, false);
	esfera1->setVelocity({ 0, -5, 0 });

	// Clones
	for (int i = 0; i < 5; i++)
		Rigid* r = createRigidEsfera("Clon" + std::to_string(i), { 0, 40 - 5 * i, 0 }, Dynamic, 10, false);

	// Esfera grande y pesada
	Rigid* esfera2 = createRigidEsfera("Esfera2", { -120, 10, 0 }, Dynamic, 1000, true, 1.0, 5.0);
	//esfera2->setVelocity({ 0, -10, 0 });
	esfera2->setMass(esfera2->getCollider()->volume() * 500); // mitad de densidad que el agua

	// - - CUBOS - - //
	// Cubo 1
	Rigid* cubo1 = createRigidCubo("Cubo1", { 10, 5, 0 }, Static, 1, false, 2.0, { 3, 3, 3 });
	// Cubo 2
	Rigid* cubo2 = createRigidCubo("Cubo2", { 10, 60, 0 }, Dynamic, 1, true);
	// Cubo 3
	Rigid* cubo3 = createRigidCubo("Cubo3", { 10, 50, 0 }, Dynamic, 1, false, 2.0, {2.0, 8.0, 2.0});

	// Trigger
	Rigid* trigger = createRigidCubo("Trigger", { 10, 20, 0 }, Dynamic, 1, false, 0.5, { 1, 1, 1});
	trigger->getCollider()->setTrigger(true);
	trigger->getEntity()->addComponent(new BloqueComp());

	// - - - MUELLE - - - //
	// Base
	Rigid* baseMuelle = createRigidCubo("BaseMuelle", { 0, 0, 15 }, Static, 1, false, 2.0, {4.0, 2.0, 4.0});
	// Extremo
	Rigid* extremoMuelle = createRigidCubo("ExtremoMuelle", { 7.5, 0, 15 }, Dynamic, 1);

	// Añadir el muelle
	PhysicsSystem::Instance()->addMuelle(new Muelle(baseMuelle, extremoMuelle, 20));

	// - -  FLOTACIÓN - - //
	// Cubo flotacion
	Rigid* cuboFlot = createRigidCubo("CuboFlot", { -110, 10, 20 }, Dynamic, 2000.0, true);

	// Líquido
	Liquido* liquido = new Liquido(-10, 1000);
	PhysicsSystem::Instance()->addForceGenerator(liquido);

	// - -  ARTICULACIONES - - //
	// Extremo 1
	Rigid* art1 = createRigidCubo("Art1", { -40, 40, 0 }, Static, 20, false);
	// Extremo 2
	Rigid* art2 = createRigidCubo("Art2", { -45, 40, 0 }, Dynamic, 20, true);

	// Añadir la articulación
	//PhysicsSystem::Instance()->addArticulacion(new Articulacion(art1, art2, Articulacion::Fija));
	//PhysicsSystem::Instance()->addArticulacion(new Articulacion(art1, art2, Articulacion::Bisagra));
	PhysicsSystem::Instance()->addArticulacion(new Articulacion(art1, art2, Articulacion::Circular));

	// Dragón
	createDragon(art2, 8, Articulacion::Circular);

	// Sombra de la pelota
	Entity* sombra = new Poligono(40, 10.0, true);
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

	/* - - Sistema de partículas - - */
	// Con 10.000, empieza a ir demasiado lento
	ParticleSystem* particleSys = new ParticleSystem(0.4, 10000, VOLUMETRIC, PARTICLE_2D);
	particleSys->setPosition({ -25, -9, 50 });
	particleSys->setTexture("sombra");
	particleSys->setParticleSpeed(0.0);
	particleSys->setLifetime(8);
	particleSys->setVolume(glm::vec3(50, 0, 50));
	particleSys->setGravity(-1);
	//particleSys->useGravity(true);
	//particleSys->setBurst(true);
	//particleSys->setLoop(false);
	particleSys->addForceGenerator(new Viento({-0.5, 0, 0}));
	//particleSys->addForceGenerator(liquido);
	// Añadirla a la escena
	AddEntity(particleSys);

	// PhysicsMan
	//Entity* phyMan = new Entity("PhysicsManager");
	PhysicsMan* phyManComp = new PhysicsMan(nullptr, sombra, liquido);
	m_camera->addComponent(phyManComp);
	//AddEntity(phyMan);

	/* - - GameManager - - */
	// Componente CameraController
	CameraController* camComp = new CameraController(m_camera);
	// Componente GameManager
	GameManager* gmComp = new GameManager(this, m_camera, nullptr, particleSys);
	AddEntity(new Entity({camComp, gmComp}, "GameManager"));


	/* - - - Efectos de postprocesado (el orden importa) - - - */

}

void PruebaFisicas::loadResources()
{
	/* Mallas */

	/* Texturas */
	ResourceManager::Instance()->loadTexture("cobre.bmp", "cobre");
	ResourceManager::Instance()->loadTexture("sombra.png", "sombra");
	ResourceManager::Instance()->loadTexture("blanco.png", "blanco");
	ResourceManager::Instance()->loadTexture("UI\\panel.png", "panel");

	ResourceManager::Instance()->loadCubemapTexture({ "skyboxes/right.jpg", "skyboxes/left.jpg", "skyboxes/bottom.jpg",
	"skyboxes/top.jpg", "skyboxes/front.jpg", "skyboxes/back.jpg" }, "lakeSkybox");

	/* Materiales */

	/* Shaders */

	/* Efectos de postprocesado ('composites') */

}

Rigid* PruebaFisicas::createRigidCubo(const std::string& name, vector3 pos, RigidType type, double masa,
	bool useGravity, double tamLado, vector3 tamCollider)
{
	// NOTA: hay que añadir el componente Collider antes de añadir el Rigid.
	// Entidad, nombre, material...
	Cubo* cubo = new Cubo(tamLado);
	cubo->setName(name);
	cubo->setTexture("default");
	cubo->setShader("lights");
	cubo->setPosition(pos);
	cubo->getComponent<Renderer>()->setActive(false);
	// Collider
	Collider* colCubo = new Collider(tamCollider);
	colCubo->setVisible(true);
	cubo->addComponent(colCubo);
	// Rigid
	Rigid* rigidCubo = new Rigid(cubo->getModelMat(), type);
	rigidCubo->setMass(masa);
	rigidCubo->useGravity(useGravity);
	cubo->addComponent(rigidCubo);
	// Añadir a la escena
	AddEntity(cubo);
	
	return rigidCubo;
}

Rigid* PruebaFisicas::createRigidEsfera(const std::string& name, vector3 pos, RigidType type, double masa,
	bool useGravity, double radio, real radioCollider)
{
	// NOTA: hay que añadir el componente Collider antes de añadir el Rigid.
	// Entidad, nombre, material...
	Esfera* esf = new Esfera(radio);
	esf->setName(name);
	esf->setTexture("default");
	esf->setShader("lights");
	esf->setPosition(pos);
	esf->getComponent<Renderer>()->setActive(false);
	// Collider
	Collider* colEsfera = new Collider(radioCollider);
	colEsfera->setVisible(true);
	esf->addComponent(colEsfera);
	// Rigid
	Rigid* rigidEsfera = new Rigid(esf->getModelMat(), type);
	rigidEsfera->setMass(masa);
	rigidEsfera->useGravity(useGravity);
	esf->addComponent(rigidEsfera);
	// Añadirla a la escena
	AddEntity(esf);

	return rigidEsfera;
}



void PruebaFisicas::createDragon(Rigid* primerNodo, int numEslabones, int tipoUnion)
{
	vector3 pos = primerNodo->getEntity()->getPosition();
	// Dragón
	Rigid* anterior = primerNodo;
	for (int i = 0; i < numEslabones; i++)
	{
		// Crear eslabón
		Rigid* siguiente = createRigidCubo("eslabon", pos - vector3(5, 0, 5), Dynamic, 20, true); //*(1.0 - i * 0.05));

		// Añadir articulación
		PhysicsSystem::Instance()->addArticulacion(new Articulacion(anterior, siguiente, (Articulacion::Type)tipoUnion));
		anterior = siguiente;
		pos = anterior->getEntity()->getPosition();
	}
}
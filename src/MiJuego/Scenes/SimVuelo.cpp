#include "SimVuelo.h"

#include "../Kepri3D.h"

#include "../GameManager.h"
#include "../RotationComp.h"
#include "../CameraController.h"

void SimVuelo::init()
{
	// CARGA DE RECURSOS PARA ESTA ESCENA
	loadResources();

	// LUCES
	// Direccional
	Light* dirLight = new Light(DIRECTIONAL_LIGHT);
	dirLight->setDirection({ 0, -1 , 0 });
	dirLight->setDiffuse({ 1, 1, 1, 1.0 });
	dirLight->setAmbient({ 0.2, 0.2, 0.2, 0.0 });
	AddLight(dirLight);

	// ENTIDADES
	// Ejes RGB
	m_entities.push_back(new EjesRGB(1.0));

	// Torre de ajedrez
	Entity* torre = new Entity("Torre");
	torre->setMesh("torre");
	torre->setPosition({ 0,-1,-2 });
	AddEntity(torre);

	// Una caja con mapa especular
	CuboSpecmap* cuboSpec = new CuboSpecmap(2);
	cuboSpec->setPosition({ 0, 0, 5 });
	cuboSpec->setTexture("caja");
	cuboSpec->setSpecularMap("caja_spec");
	//cuboSpec->setMaterial("cromo");
	AddEntity(cuboSpec);

	// Matojos de hierba (billboards)
	int numMatojos = 20;
	glm::vec3 posMatojo = { 40, -11, 44 };
	float radioMax = 15.0f;
	for(int i = 0; i < numMatojos; i++)
	{
		glm::vec3 incr = { (std::rand() % 100) / (100 / radioMax), 0, (std::rand() % 100) / (100 / radioMax) };
		Billboard* bill = new Billboard("hierba", 2, 2.5);
		bill->setPosition(posMatojo + incr);
		AddEntity(bill);
	}

	// Árbol (billboard)
	Billboard* tree = new Billboard("tree", 8, 10);
	tree->setPosition({ 85, -0.3, 57 });
	tree->setAncla({ 0, -4.25 });
	AddEntity(tree);

	/* - - Canvas - - */
	m_canvas = new Canvas();
	m_canvas->setSize(800, 600);

	/* - - Skybox - - */
	Skybox* sky = new Skybox("lakeSkybox");
	SetSkybox(sky);

	/* - - GameManager - - */
	Entity* gm = new Entity();
	// Componente CameraController
	CameraController* camComp = new CameraController(m_camera);
	gm->addComponent(camComp);
	AddEntity(gm);


	/* - - - Efectos de postprocesado (el orden importa) - - - */
	//AddComposite((Shader*)&ResourceManager::Instance()->getComposite("byn"));
	//AddComposite((Shader*)&ResourceManager::Instance()->getComposite("waves"));
	AddComposite((Shader*)&ResourceManager::Instance()->getComposite("interference"));
}

void SimVuelo::loadResources()
{
	/* Mallas */
	ResourceManager::Instance()->loadMesh("Torre.obj", "torre");

	/* Texturas */
	ResourceManager::Instance()->loadTexture("earth24.bmp", "earth");
	ResourceManager::Instance()->loadTexture("venus.bmp", "venus");
	ResourceManager::Instance()->loadTexture("Zelda.bmp", "zelda");
	ResourceManager::Instance()->loadTexture("terrenoTex.bmp", "terreno");
	ResourceManager::Instance()->loadTexture("iceland.bmp", "iceland", { 4, 65, 137 });
	ResourceManager::Instance()->loadTexture("Caja.bmp", "caja");
	ResourceManager::Instance()->loadTexture("caja_specular.bmp", "caja_spec");
	ResourceManager::Instance()->loadTexture("cobre.bmp", "cobre");
	ResourceManager::Instance()->loadTexture("agua.jpg", "agua");
	ResourceManager::Instance()->loadTexture("agua_disp.jpg", "agua_disp");
	ResourceManager::Instance()->loadTexture("grass.bmp", "hierba", { 0,0,0 });
	ResourceManager::Instance()->loadTexture("tree.png", "tree");
	ResourceManager::Instance()->loadTexture("brickwall.jpg", "wall");
	ResourceManager::Instance()->loadTexture("brickwall_normal.png", "wall_normal");


	// Skyboxes - el orden tiene que ser este (top y bottom están invertidos por alguna razón)
	ResourceManager::Instance()->loadCubemapTexture({ "skyboxes/right.jpg", "skyboxes/left.jpg", "skyboxes/bottom.jpg",
		"skyboxes/top.jpg", "skyboxes/front.jpg", "skyboxes/back.jpg" }, "lakeSkybox");

	/* Materiales */
	ResourceManager::Instance()->loadMaterial("copper.material", "cobre");
	ResourceManager::Instance()->loadMaterial("crome.material", "cromo");
	ResourceManager::Instance()->loadMaterial("fluorescente.material", "fluorescente");
	ResourceManager::Instance()->loadMaterial("oceano.material", "oceano");


	/* Shaders */
	ResourceManager::Instance()->loadShader("maximize.vert", "", "fog.frag", "bigFog");
	ResourceManager::Instance()->loadShader("default.vert", "", "movimiento.frag", "movimiento");
	ResourceManager::Instance()->loadShader("default.vert", "", "multitexture.frag", "multitexture");
	ResourceManager::Instance()->loadShader("lights.vert", "", "specularMap.frag", "specMap");
	ResourceManager::Instance()->loadShader("lights.vert", "", "normalMap.frag", "normalMap");
	ResourceManager::Instance()->loadShader("clippable.vert", "", "default.frag", "clippable");
	ResourceManager::Instance()->loadShader("default.vert", "terrain.tesc", "terrain.tese", "", "default.frag", "terreno");


	/* Efectos de postprocesado ('composites') */
	ResourceManager::Instance()->loadComposite("interference.frag", "interference");
	ResourceManager::Instance()->loadComposite("waves.frag", "waves");
	ResourceManager::Instance()->loadComposite("byn.frag", "byn");
}

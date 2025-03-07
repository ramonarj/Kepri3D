#include "PruebaScene.h"

#include "../Kepri3D.h"

#include "../GameManager.h"
#include "../RotationComp.h"
#include "../CameraController.h"
#include "../MultitexComp.h"
#include "../AguaComp.h"
#include "../DebugText.h"
#include "../Callbacks.h"

const float buttonScale = 0.27f;
const float buttonMargin = 0.1f;
const float buttonSep = 0.12f;

std::vector<std::string> buttonNames = { "botonCulling", "botonBlending", "botonLighting", "botonTextures", "botonShading",
"botonAlpha", "botonMultisampling", "botonMipmaps", "botonNormales", "botonPostprocess", "botonScissor",
"botonSkybox", "botonGamma", "botonInstancing", "botonStencil", "botonLogicOp"};

std::vector<std::string> buttonTexts = { "CULLING", "BLENDING", "LIGHTING", "TEXTURES", "FLAT\nSHADING",
"ALPHA\nTEST", "MULTI\nSAMPLING", "MIPMAPS", "VER\nNORMALES", "POST\nPROCESADO", "SCISSOR\nTEST",
"SKYBOX", "FRESNEL\nEFFECT", "INSTANCING", "SOMBRAS", "LOGIC\nOP" };

std::vector<void(*)()> PruebaScene::callbacks = { cullingButtonPressed, blendingButtonPressed, lightingButtonPressed,
texturesButtonPressed, shadingButtonPressed, alphaButtonPressed, multisamplingButtonPressed, mipmapButtonPressed,
normalsButtonPressed, compositeButtonPressed, scissorButtonPressed, skyboxButtonPressed, fresnelButtonPressed, 
instancingButtonPressed, shadowsButtonPressed, logicOpButtonPressed, gammaButtonPressed };


void PruebaScene::init()
{
	// LUCES
	// Direccional
	Light* dirLight = new Light(DIRECTIONAL_LIGHT);
	dirLight->setDirection({ -1,0 , 0 });
	dirLight->setDiffuse({ 1, 1, 1, 1.0 });
	dirLight->setAmbient({ 0.05, 0.05, 0.2, 0.0 });
	dirLight->setActive(true);
	Entity* dirLightEnt = new Entity();
	dirLightEnt->addComponent(dirLight);
	AddEntity(dirLightEnt);

	// Puntual (va en círculos)
	Light* circleLight = new Light(POINT_LIGHT, { 1, 1, 0, 1 });
	circleLight->setAmbient({ 0.1, 0.3, 0.1, 1.0 });
	circleLight->setSpecular({ 0.8, 0.8, 0, 1.0 });
	circleLight->emitShadows(true);
	Entity* circleLightEnt = new Esfera(0.5);
	circleLightEnt->setTexture("blanco");
	circleLightEnt->setShader("default");
	circleLightEnt->addComponent(circleLight);
	AddEntity(circleLightEnt);

	// Foco (linterna)
	Light* spotLight = new Light(SPOT_LIGHT, { 0.7, 0.7, 0.7, 1 });
	spotLight->setSpecular({ 0.6, 0.6, 0.6, 1.0 });
	Entity* spotLightEnt = new Entity();
	spotLightEnt->addComponent(spotLight);
	AddEntity(spotLightEnt);

	// Nueva luz para el Blinn
	Light* luzBlinn = new Light(POINT_LIGHT, { 0.4, 0.4, 0.3, 1 });
	luzBlinn->setAttenuationFactors(0.3, 0.3, 0.02);
	Entity* e = new Entity({ luzBlinn }, "LuzBlinn");
	e->setPosition({ 60, 4, -40 });
	AddEntity(e);

	// ENTIDADES
	// Ejes RGB
	//m_entities.push_back(new EjesRGB(0.5));

	// Polígono sin rellenar
	AddEntity(new Poligono(6, 0.5, false));

	// Polígono relleno
	Poligono* pol = new Poligono(4, 1, true);
	pol->setTexture("caja");
	//pol->setMaterial("default");
	AddEntity(pol);

	// Materiales y cubos
	PruebaMateriales();

	// Estrella del Mario 64
	Entity* estrella = new Entity("Estrella");
	estrella->setMesh("star");
	estrella->setTexture("star");
	estrella->setShader("default");
	estrella->setPosition({ -5,2,-6 });
	AddEntity(estrella);
	estrella->addComponent(new RotationComp(1.0));

	// Redead del Zelda OoT
	Entity* redead = new Entity("Redead");
	redead->setMesh("redead");
	redead->setMaterial("fluorescente");
	redead->setTexture("redead");
	redead->setShader("lights");
	redead->setPosition({ -8,2,-6 });
	AddEntity(redead);

	// Peon de ajedrez
	Entity* peon = new Entity("Peon");
	peon->setMesh("peon");
	peon->setMaterial("cristal");
	peon->setShader("lights");
	peon->setPosition({ 0,0,-2 });
	AddEntity(peon);


	// Torre de ajedrez
	Entity* torre = new Entity("Torre");
	torre->setMesh("torre");
	//torre->setTexture("cobre");
	torre->setMaterial("ruby");
	torre->setShader("lights");
	torre->setPosition({ 0,15,-15 });
	torre->addComponent(new RotationComp(0.5));
	AddEntity(torre);
	// Para que el blending del rubí funcione bien (de momento), 
	// tiene que estar la última de las entidades

	// Esferita hija de la torre
	Esfera* esferita = new Esfera(1.0, 5, 10);
	esferita->setTexture("earth");
	esferita->setPosition({ 0,2,-6 });
	esferita->addComponent(new RotationComp(1));
	esferita->setParent(torre);
	//AddEntity(esferita);

	// Un toro
	Toro* toro = new Toro(0.5, 0.25, 20, 10);
	toro->setPosition({ 2,0,-2 });
	toro->setMaterial("fluorescente");
	toro->setParent(esferita);
	//toro->setShader("lights");
	//AddEntity(toro);

	// Un cilindro
	Cilindro* cilindro = new Cilindro(2, 4);
	cilindro->setPosition({ 24,-0.9,-10 });
	cilindro->setMaterial("cromo");
	cilindro->setTexture("coke");
	cilindro->setShader("lights");
	AddEntity(cilindro);

	// Un cubo multitextura
	Cubo* cuboMT = new Cubo(2);
	cuboMT->setName("CuboMT");
	cuboMT->setPosition({ -15, 0, 6 });
	cuboMT->setTexture("lego");
	cuboMT->setSecondTex("emoji");
	cuboMT->getRenderer()->castShadows(false);
	cuboMT->setShader("multitexture");
	cuboMT->addComponent(new MultitexComp((Material*)cuboMT->getMaterial()));
	AddEntity(cuboMT);

	// Una caja con mapa especular
	Cubo* cuboSpec = new Cubo(2);
	cuboSpec->setName("CuboSpecmap");
	cuboSpec->setPosition({ 0, 0, 5 });
	cuboSpec->setTexture("caja");
	cuboSpec->setSpecularMap("caja_spec");
	cuboSpec->setShader("lights");
	AddEntity(cuboSpec);

	// Hierba
	Hierba* hierba = new Hierba(2.5, 3);
	hierba->setTexture("hierba");
	hierba->setPosition({ 15, 0.5, 5 });
	hierba->rotate(-PI / 2, { 0, 1, 0 }, GLOBAL);
	hierba->addComponent(new RotationComp(0.75));
	AddEntity(hierba);


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

	// Textos de prueba
	//ABCDEFGHIJKLMNOPQRSTUVWXYZ
	//Text* texto = new Text("EN UN LUGAR DE LA MANCHA", m_canvas, { 0, 1, 0, 1 });
	//texto->setPositionUI(0.15, 0.1);
	//m_canvas->addElement(texto);

	//Text* textoNums = new Text("0123456789-/", m_canvas, { 0, 1, 0, 1 });
	//textoNums->setPositionUI(0.3, 0.05);
	//textoNums->setGrosor(2.0);
	//m_canvas->addElement(textoNums);

	// Entidad vacía para que cuelguen los botones
	UIElement* botonesMenu = new UIElement();
	botonesMenu->setActive(false);
	m_canvas->addElement(botonesMenu);

	// Botones
	for (int i = 0; i < buttonNames.size(); i++)
	{
		// Su textura, posición, escala y callback
		Button* b = new Button("boton", m_canvas);
		b->setName(buttonNames[i]);
		glm::vec2 pos;
		if (i < 8)
			pos = { buttonMargin, 0.92 - i * buttonSep };
		else
			pos = { 1 - buttonMargin, 0.92 - (i - 8) * buttonSep };
		b->setPositionUI(pos.x, pos.y);
		b->setScaleUI(buttonScale, buttonScale);
		b->setCallback(callbacks[i]);
		b->setParent(botonesMenu); // hijos del nodo vacío

		// El texto que contiene
		Text* t = new Text(buttonTexts[i], m_canvas, {1, 1, 1, 1});
		t->setPositionUI(pos.x - 0.06, pos.y);
		t->setScaleUI(buttonScale * 2, buttonScale * 2);
		t->setGrosor(1.5);
		t->setParent(b);
	}


	/* - - Skybox - - */
	Skybox* sky = new Skybox("lakeSkybox");
	SetSkybox(sky);

	/* - - Sistema de partículas - - */
	// Con 10.000, empieza a ir demasiado lento
	ParticleSystem* particleSys = new ParticleSystem(0.4, 1000, PARTICLE_2D);
	particleSys->setTexture("emoji");
	particleSys->setParticleSpeed(3.0);
	particleSys->setLifetime(2);
	particleSys->setActive(false);
	AddEntity(particleSys);

	// Terreno teselado
	TessTerrain* tesTerrain = new TessTerrain(9, 14, 80, 80); //patches de 80x80uds
	tesTerrain->setTexture("iceland");
	tesTerrain->setHeightMap("iceland_height", 25.0f);
	tesTerrain->setSpecularMap("iceland_spec");
	tesTerrain->setNormalMap("iceland_normal");  // obligatorio usar un mapa de normales por el momento
	tesTerrain->setPosition({ 0,-20,0 });
	tesTerrain->setCamera(m_camera);
	tesTerrain->getRenderer()->setPolygonMode(GL_FILL, GL_LINE);
	AddEntity(tesTerrain);

	// Información de Debug
	Entity* debugTxt = new Entity("DebugText");
	debugTxt->setPosition({ 0,0,0 });
	debugTxt->addComponent(new DebugText(m_canvas, "panel"));
	AddEntity(debugTxt);
	debugTxt->setActive(true);

	/* - - GameManager - - */
	// Componente GM
	GameManager* gmComponent = new GameManager(this, m_camera, botonesMenu, torre);
	gmComponent->setLights(dirLight, circleLight, spotLight, luzBlinn);
	gmComponent->setTessTerrain(tesTerrain);
	gmComponent->setParticleSys(particleSys);
	// Componente CameraController
	CameraController* camComp = new CameraController(m_camera);
	Entity* gm = new Entity({ gmComponent, camComp }, "GameManager");
	AddEntity(gm);

	/* - - - Efectos de postprocesado (el orden importa) - - - */
	//AddComposite((Shader*)&ResourceManager::Instance()->getComposite("byn"));
	//AddComposite((Shader*)&ResourceManager::Instance()->getComposite("waves"));
	AddComposite((Shader*)&ResourceManager::Instance()->getComposite("bordes"));
	AddComposite((Shader*)&ResourceManager::Instance()->getComposite("interference"));

	// Temporal
	// Establecer el punto de enlace de los shaders que usen el UBO
	for (Entity* e : m_entities)
	{
		Shader* sh = (Shader*)e->getShader();
		if (sh != nullptr)
		{
			sh->bindUniformBlock("Matrices", 0);
			sh->bindUniformBlock("Lights", 1);
		}
	}
}

void PruebaScene::loadResources()
{
	/* Mallas */
	ResourceManager::Instance()->loadMesh("Torre.obj", "torre");
	ResourceManager::Instance()->loadMesh("Peon.obj", "peon");
	ResourceManager::Instance()->loadMesh("star.obj", "star");
	ResourceManager::Instance()->loadMesh("redead.obj", "redead", 0.1f);

	/* Texturas */
	ResourceManager::Instance()->loadTexture("earth24.bmp", "earth");
	ResourceManager::Instance()->loadTexture("earthHD.jpg", "earthHD");
	ResourceManager::Instance()->loadTexture("venus.bmp", "venus");
	ResourceManager::Instance()->loadTexture("orientacion.bmp", "orientacion");
	ResourceManager::Instance()->loadTexture("Zelda.bmp", "zelda");
	ResourceManager::Instance()->loadTexture("terrenoTex.bmp", "terreno");
	ResourceManager::Instance()->loadTexture("Caja.bmp", "caja");
	ResourceManager::Instance()->loadTexture("cajaPrueba.png", "cajaPrueba");
	ResourceManager::Instance()->loadTexture("caja_specular.bmp", "caja_spec");
	ResourceManager::Instance()->loadTexture("cobre.bmp", "cobre");
	ResourceManager::Instance()->loadTexture("agua.jpg", "agua");
	ResourceManager::Instance()->loadTexture("agua_disp.jpg", "agua_disp");
	ResourceManager::Instance()->loadTexture("emoji.bmp", "emoji", { 0, 0, 0 });
	ResourceManager::Instance()->loadTexture("lego.bmp", "lego", 100);
	ResourceManager::Instance()->loadTexture("grass.bmp", "hierba", { 0,0,0 });
	ResourceManager::Instance()->loadTexture("tree.png", "tree");
	ResourceManager::Instance()->loadTexture("brickwall.jpg", "wall");
	// Los mapas de normales/alturas no deben cargarse con SRGB_ALPHA (los que no representen colores)
	ResourceManager::Instance()->loadTexture("brickwall_normal.png", "wall_normal", 255, GL_RGBA);
	ResourceManager::Instance()->loadTexture("star.png", "star");
	ResourceManager::Instance()->loadTexture("redead.png", "redead");
	ResourceManager::Instance()->loadTexture("windows.bmp", "windows");
	ResourceManager::Instance()->loadTexture("coke.jpg", "coke");
	ResourceManager::Instance()->loadTexture("espejo.png", "espejo");
	ResourceManager::Instance()->loadTexture("espejo_reflection.png", "espejo_reflection");
	ResourceManager::Instance()->loadTexture("blanco.png", "blanco"); //1px blanco

	// Texturas del terreno
	//ResourceManager::Instance()->loadTexture("iceland.bmp", "iceland", { 4, 65, 137 });
	ResourceManager::Instance()->loadTexture("iceland.bmp", "iceland", { 4, 65, 137 });
	ResourceManager::Instance()->loadTexture("iceland_height_highres.png", "iceland_height", 255, GL_RGBA);
	ResourceManager::Instance()->loadTexture("iceland_normal.png", "iceland_normal", 255, GL_RGBA);
	ResourceManager::Instance()->loadTexture("iceland_spec.png", "iceland_spec");

	// Canvas
	ResourceManager::Instance()->loadTexture("UI\\boton.png", "boton");
	ResourceManager::Instance()->loadTexture("UI\\panel.png", "panel");

	// Skyboxes - el orden tiene que ser este (top y bottom están invertidos por alguna razón)
	ResourceManager::Instance()->loadCubemapTexture({ "skyboxes/right.jpg", "skyboxes/left.jpg", "skyboxes/bottom.jpg",
		"skyboxes/top.jpg", "skyboxes/front.jpg", "skyboxes/back.jpg" }, "lakeSkybox");
	ResourceManager::Instance()->loadCubemapTexture({ "skyboxes/city/right.jpg", "skyboxes/city/left.jpg", "skyboxes/city/bottom.jpg",
		"skyboxes/city/top.jpg", "skyboxes/city/front.jpg", "skyboxes/city/back.jpg" }, "citySkybox");

	/* Shaders (ES IMPORTANTE CARGARLOS ANTES QUE LOS MATERIALES) */
	ResourceManager::Instance()->loadShader("default.vert", "cruces.geom", "default.frag", "cruces");
	ResourceManager::Instance()->loadShader("maximize.vert", "", "fog.frag", "bigFog");
	ResourceManager::Instance()->loadShader("default.vert", "", "multitexture.frag", "multitexture");
	ResourceManager::Instance()->loadShader("clippable.vert", "", "default.frag", "clippable");
	ResourceManager::Instance()->loadShader("terrain.vert", "terrain.tesc", "terrain.tese", "", "lights.frag", "terreno");
	ResourceManager::Instance()->loadShader("reflejos.vert", "", "reflejos.frag", "reflejos");

	/* Materiales */
	ResourceManager::Instance()->loadMaterial("copper.material", "cobre");
	ResourceManager::Instance()->loadMaterial("crome.material", "cromo");
	ResourceManager::Instance()->loadMaterial("ruby.material", "ruby");
	ResourceManager::Instance()->loadMaterial("cristal.material", "cristal");
	ResourceManager::Instance()->loadMaterial("fluorescente.material", "fluorescente");
	ResourceManager::Instance()->loadMaterial("blinn.material", "blinn");
	ResourceManager::Instance()->loadMaterial("oceano.material", "oceano");
	ResourceManager::Instance()->loadMaterial("emerald.material", "emerald");

	// Prueba excepciones
	ResourceManager::Instance()->loadTexture("ladrillo.bmp", "ladrillo");
	ResourceManager::Instance()->loadMaterial("plata.material", "plata");

	/* Efectos de postprocesado ('composites') */
	ResourceManager::Instance()->loadComposite("interference.frag", "interference");
	ResourceManager::Instance()->loadComposite("waves.frag", "waves");
	ResourceManager::Instance()->loadComposite("byn.frag", "byn");
	ResourceManager::Instance()->loadComposite("bordes.frag", "bordes");
}


void PruebaScene::PruebaMateriales()
{

	// Cubo de orientación (distintas texturas)
	Cubo* c2 = new Cubo(2, false);
	c2->setMaterial("fluorescente");
	c2->setTexture("orientacion");
	c2->setPosition({ -5,0,0 });
	//c2->setShader("lights");
	AddEntity(c2);

	// Cubo con la misma textura en todas las caras
	Cubo* c = new Cubo(2);
	c->setTexture("cajaPrueba");
	c->setPosition({ -10,0,0 });
	c->setShader("lights");
	AddEntity(c);

	// Cubo default
	//Cubo* cuboDef = new Cubo(2);
	//cuboDef->setMaterial("default");
	//cuboDef->setPosition({ 0,0,0 });
	//AddEntity(cuboDef);

	// Cubo de cromo
	Cubo* cuboCromo = new Cubo(2);
	//cuboCromo->setMaterial("cromo");
	cuboCromo->setPosition({ 5,0,0 });
	AddEntity(cuboCromo);

	// Cubo de cobre
	Cubo* cuboCobre = new Cubo(2);
	cuboCobre->setTexture("cobre");
	cuboCobre->setMaterial("cobre");
	cuboCobre->setPosition({ 10,0,0 });
	AddEntity(cuboCobre);

	// Esfera
	Esfera* esfera = new Esfera(1, 8, 16);
	esfera->setTexture("earth");
	esfera->setPosition({ -15,2,-3 });
	esfera->setShader("bigFog");
	AddEntity(esfera);

	// Tierra
	Esfera* tierra = new Esfera(3);
	tierra->setName("TierraCruces");
	tierra->addComponent(new RotationComp(1.0));
	tierra->setTexture("earth");
	tierra->setPosition({ 6,15,0 });
	tierra->rotate(-PI / 8, { 0, 0, 1 }, GLOBAL);
	tierra->setShader("cruces");
	AddEntity(tierra);

	// Rejilla (suelo)
	Grid* grid = new Grid(1, 1, 30, 60);
	//grid->setMaterial("oceano");
	grid->setTexture("agua");
	grid->setDisplacementMap("agua_disp");
	grid->enableReflections("blanco", "lakeSkybox");
	grid->setShader("agua");
	grid->setPosition({ 0,-1,0 });
	AguaComp* sueloComp = new AguaComp((Material*)grid->getMaterial());
	sueloComp->setSpeeds({ 2.0, 0.0 }, { -2, 0 });
	grid->addComponent(sueloComp);
	AddEntity(grid);

	// "Cascada"
	Grid* cascada = new Grid(1, 1, 30, 24);
	cascada->setName("Cascada");
	cascada->setMaterial("oceano");
	cascada->setTexture("agua");
	cascada->setDisplacementMap("agua_disp");
	cascada->enableReflections("blanco", "lakeSkybox");
	cascada->setPosition({ 30,-13,0 });
	cascada->rotate(-PI / 2, { 0, 0, 1 }, GLOBAL);
	cascada->setShader("agua");
	AguaComp* cascadaComp = new AguaComp((Material*)cascada->getMaterial());
	cascadaComp->setSpeeds({ 5.0, 0.0 }, { 10.0, 0.0 }); 
	cascada->addComponent(cascadaComp);
	AddEntity(cascada);

	// Terreno antiguo
	//Terrain* oldTer = new Terrain();
	//oldTer->loadRAW(ResourceManager::ASSETS_PATH + "terrain.raw", 0.05);
	//oldTer->setTexture("terreno");
	//oldTer->setPosition({ -30,0, -15 });
	//AddEntity(oldTer);

	// Terreno Islandia
	//Terrain* terrain = new Terrain();
	//terrain->loadHeightMap(ResourceManager::TEXTURES_PATH + "iceland_height.bmp", 2.0);
	//terrain->setTexture("iceland");
	//terrain->setShader("lights");
	//terrain->setPosition({ 0,-20,0 });
	//AddEntity(terrain);


	// Mar de la isla
	Grid* mar = new Grid(1, 1, 723 * 3, 1087 * 3);
	mar->setMaterial("oceano");
	mar->setTexture("agua");
	mar->setDisplacementMap("agua_disp");
	mar->enableReflections("blanco", "lakeSkybox");
	mar->setPosition({ 0,-20.01,0 });
	mar->setShader("agua");
	AguaComp* marComp = new AguaComp((Material*)mar->getMaterial());
	marComp->setSpeeds({ 0.0, 0.0 }, { -0.2, -0.2 });
	mar->addComponent(marComp);
	AddEntity(mar);

	// Minimap
	//Terrain* miniTer = new Terrain();
	//miniTer->loadHeightMap("../../bin/assets/icelandHeight.bmp", 0.01);
	//miniTer->setTexture("iceland");
	//miniTer->scale({ 1,1.5,1 });
	//miniTer->setPosition({ -6,-0.99,6 });
	//miniTer->rotate(PI / 2, { 0,1,0 }, GLOBAL);
	//AddEntity(miniTer);

	// Pared de ladrillos (normal maps)
	Grid* pared = new Grid(1, 1, 15, 15);
	pared->setName("NormalMapWall");
	pared->setMaterial("cromo");
	pared->setTexture("wall");
	pared->setNormalMap("wall_normal");
	pared->setShader("lights");
	pared->setPosition({ -5,6.5,-15 });
	pared->rotate(PI / 2, { 1,0,0 });
	//pared->addComponent(new RotationComp(0.5));
	AddEntity(pared);

	// Pared sin normal map para comparar
	Grid* pared2 = new Grid(1, 1, 15, 15);
	pared2->setTexture("wall");
	pared2->setShader("lights");
	pared2->setPosition({ 10,6.5,-15 });
	pared2->rotate(PI / 2, { 1,0,0 });
	//pared2->receiveShadows(false);
	//pared2->addComponent(new RotationComp(0.5));
	AddEntity(pared2);

	// Plano para iluminación Blinn-Phong
	Grid* plano = new Grid(1, 1, 30, 30);
	plano->setMaterial("blinn");
	plano->setTexture("cobre");
	plano->setShader("lights");
	plano->setPosition({ 60,0,-40 });
	AddEntity(plano);

	// Toro para clip planes
	ClippableEntity* cortado = new ClippableEntity();
	cortado->setTexture("default");
	cortado->setPosition({ -25,1,5 });
	AddEntity(cortado);

	// Esfera HD
	Esfera* esf = new Esfera(10, 30, 40);
	//esf->setTexture("earthHD");
	esf->setPosition({ 0,10,25 });
	esf->setMaterial("emerald");
	esf->setShader("lights");
	esf->addComponent(new RotationComp(0.25));
	AddEntity(esf);

	// Cubo con reflejos
	Cubo* cuboReflejos = new Cubo(6.0);
	cuboReflejos->setTexture("citySkybox");
	cuboReflejos->enableReflections("blanco", "citySkybox");
	cuboReflejos->setPosition({ 40,3,0 });
	cuboReflejos->setShader("reflejos");
	cuboReflejos->addComponent(new RotationComp(0.5));
	AddEntity(cuboReflejos);

	// Esfera con reflejos
	Esfera* esfReflejos = new Esfera(6, 30, 40);
	esfReflejos->setTexture("lakeSkybox");
	esfReflejos->enableReflections("blanco", "lakeSkybox");
	esfReflejos->setPosition({ 40,25,0 });
	esfReflejos->setShader("reflejos");
	AddEntity(esfReflejos);

	// Espejo
	Grid* espejo = new Grid(1, 1, 4, 2);
	espejo->setName("espejo");
	espejo->setTexture("espejo");
	espejo->enableReflections("espejo_reflection", "citySkybox");
	espejo->setPosition({ 25,3,0 });
	espejo->setShader("reflejos");
	espejo->rotate(PI / 2.0, { 1, 0, 0 });
	AddEntity(espejo);

	// Prueba para sombras
	Grid* tejado = new Grid(1, 1, 15, 15);
	tejado->setTexture("cobre");
	tejado->setPosition({ 0,10,0 });
	tejado->setShader("lights");
	AddEntity(tejado);

	// Entidad con VBO
	VBOEntity* vboEnt = new VBOEntity();
	vboEnt->setTexture("zelda");
	vboEnt->setShader("lights");
	vboEnt->setPosition({ 0,1,-5 });
	AddEntity(vboEnt);

	// Entidad con EBO
	EBOEntity* eboEnt = new EBOEntity();
	eboEnt->setTexture("zelda");
	eboEnt->setShader("lights");
	eboEnt->setPosition({ 5,1,-5 });
	AddEntity(eboEnt);

	// Entidad con VAO
	VAOEntity* vaoEnt = new VAOEntity();
	//vaoEnt->setMaterial("ruby");
	vaoEnt->setTexture("zelda");
	vaoEnt->setShader("lights");
	vaoEnt->setPosition({ 10,1,-5 });
	AddEntity(vaoEnt);
}

void PruebaScene::ViewportTest()
{
	Shader::turnOff();

	//Algunas variables locales para facilitar la puesta en escena
	Viewport* view = m_camera->getVP();
	GLdouble w = view->getW();
	GLdouble h = view->getH();
	// Reducimos el tamaño del puerto a 1/4 de la pantalla y dibujamos en los 3 primeros cuadrantes
	view->setSize(w / 2, h / 2);

	// Ejes RGB
	view->setPosition(0, h / 2);
	m_entities[0]->render(m_camera->getViewMat());

	// Polígono sin rellenar
	view->setPosition(w / 2, h / 2);
	m_entities[1]->render(m_camera->getViewMat());

	// Cubo texturizado
	view->setPosition(0, 0); // 0,0 es abajo a la izquierda
	m_entities[2]->render(m_camera->getViewMat());
	// Podemos dibujar un mismo objeto todas las veces que queramos (ejes extra)
	m_entities[0]->render(m_camera->getViewMat());

	// Cubo orientación
	view->setPosition(w / 2, 0);
	m_entities[3]->render(m_camera->getViewMat());

	view->setSize(w, h); //Volvemos a dejar el viewPort como estaba
}
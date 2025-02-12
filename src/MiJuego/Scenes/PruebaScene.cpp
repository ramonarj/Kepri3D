#include "PruebaScene.h"

#include "../Kepri3D.h"

#include "../GameManager.h"
#include "../RotationComp.h"
#include "../CameraController.h"
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
"SKYBOX", "GAMMA\nCORRECTION", "INSTANCING", "STENCIL\nTEST", "LOGIC\nOP" };

std::vector<void(*)()> PruebaScene::callbacks = { cullingButtonPressed, blendingButtonPressed, lightingButtonPressed,
texturesButtonPressed, shadingButtonPressed, alphaButtonPressed, multisamplingButtonPressed, mipmapButtonPressed,
normalsButtonPressed, compositeButtonPressed, scissorButtonPressed, skyboxButtonPressed, gammaButtonPressed, 
instancingButtonPressed, stencilButtonPressed, logicOpButtonPressed };


void PruebaScene::init()
{
	// LUCES
	// Direccional
	Light* dirLight = new Light(DIRECTIONAL_LIGHT);
	dirLight->setDirection({ -1,0 , 0 });
	dirLight->setDiffuse({ 1, 1, 1, 1.0 });
	dirLight->setAmbient({ 0.2, 0.2, 0.4, 0.0 });
	dirLight->setActive(true);
	AddLight(dirLight);

	// Puntual (va en c�rculos)
	Light* circleLight = new Light(POINT_LIGHT, { 1, 1, 0, 1 });
	circleLight->setPosition({ 3,3,-3 });
	circleLight->setAmbient({ 0.1, 0.3, 0.1, 1.0 });
	circleLight->setSpecular({ 1, 1, 0, 1.0 });
	AddLight(circleLight);

	// Foco (linterna)
	Light* spotLight = new Light(SPOT_LIGHT, { 0.7, 0.7, 0.7, 1 });
	spotLight->setSpecular({ 1, 1, 1, 1.0 });
	AddLight(spotLight);

	// Nueva luz para el Blinn
	Light* luzBlinn = new Light(POINT_LIGHT, { 0.4, 0.4, 0.3, 1 });
	luzBlinn->setPosition({ 60, 4, 0 });
	luzBlinn->setAttenuationFactors(0.3, 0.03, 0.002);
	AddLight(luzBlinn);


	// ENTIDADES
	// Ejes RGB
	//m_entities.push_back(new EjesRGB(0.5));

	// Pol�gono sin rellenar
	AddEntity(new Poligono(6, 0.5, false));

	// Pol�gono relleno
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
	Entity* redead = new Entity("Estrella");
	redead->setMesh("redead");
	redead->setTexture("redead");
	redead->setShader("default");
	redead->setPosition({ -8,2,-6 });
	redead->scale({ 0.1, 0.1, 0.1 });
	AddEntity(redead);

	// Peon de ajedrez
	Entity* peon = new Entity("Peon");
	peon->setMesh("peon");
	peon->setMaterial("cristal");
	peon->setPosition({ 0,0,-2 });
	AddEntity(peon);


	// Torre de ajedrez
	Entity* torre = new Entity("Torre");
	torre->setMesh("torre");
	//torre->setTexture("cobre");
	torre->setMaterial("ruby");
	torre->setPosition({ 0,-1,-2 });
	AddEntity(torre);
	// Para que el blending del rub� funcione bien (de momento), 
	// tiene que estar la �ltima de las entidades

	// Esferita hija de la torre
	Esfera* esferita = new Esfera(0.5, 5, 10);
	//esferita->setTexture("earth");
	esferita->setPosition({ 0,3.5,-2 });
	esferita->setParent(torre);
	//AddEntity(esferita);

	// Un toro
	Toro* toro = new Toro(0.75, 0.4, 30, 10);
	toro->setPosition({ 20,-0.4,6 });
	toro->setMaterial("fluorescente");
	AddEntity(toro);

	// Un cilindro
	Cilindro* cilindro = new Cilindro(2, 4);
	cilindro->setPosition({ 24,-0.9,-10 });
	cilindro->setTexture("coke");
	cilindro->setMaterial("cromo");
	cilindro->setShader("lights");
	AddEntity(cilindro);

	// Un cubo multitextura
	CuboMultitex* cuboMT = new CuboMultitex(2);
	cuboMT->setPosition({ -15, 0, 6 });
	cuboMT->setTexture("lego");
	cuboMT->setSecondTex("emoji");
	AddEntity(cuboMT);

	// Una caja con mapa especular
	CuboSpecmap* cuboSpec = new CuboSpecmap(2);
	cuboSpec->setPosition({ 0, 0, 5 });
	cuboSpec->setTexture("caja");
	cuboSpec->setSpecularMap("caja_spec");
	//cuboSpec->setMaterial("cromo");
	AddEntity(cuboSpec);

	// Hierba
	Hierba* hierba = new Hierba(2.5, 3);
	hierba->setPosition({ 15, 0.5, 5 });
	hierba->rotate(-PI / 2, { 0, 1, 0 }, GLOBAL);
	hierba->setTexture("hierba");
	//hierba->setShader("lights");
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

	// �rbol (billboard)
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

	// Entidad vac�a para que cuelguen los botones
	UIElement* botonesMenu = new UIElement();
	botonesMenu->setActive(false);
	m_canvas->addElement(botonesMenu);

	// Botones
	for (int i = 0; i < buttonNames.size(); i++)
	{
		// Su textura, posici�n, escala y callback
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
		b->setParent(botonesMenu); // hijos del nodo vac�o

		// El texto que contiene
		Text* t = new Text(buttonTexts[i], m_canvas, {1, 1, 1, 1});
		t->setPositionUI(pos.x - 0.06, pos.y);
		t->setScaleUI(buttonScale * 2, buttonScale * 2);
		t->setGrosor(1.5);
		t->setParent(b);
	}


	/* - - Skybox - - */
	Skybox* sky = new Skybox("citySkybox");
	SetSkybox(sky);

	/* - - Sistema de part�culas - - */
	// Con 10.000, empieza a ir demasiado lento
	ParticleSystem* particleSys = new ParticleSystem(0.4, 1000, PARTICLE_2D);
	particleSys->setTexture("emoji");
	particleSys->setParticleSpeed(3.0);
	particleSys->setLifetime(2);
	particleSys->setActive(false);
	AddEntity(particleSys);

	// Terreno teselado
	TessTerrain* tesTerrain = new TessTerrain(2, 2, 30, 30);
	tesTerrain->loadHeightMap(ResourceManager::TEXTURES_PATH + "iceland_height.bmp", 2.0);
	tesTerrain->setTexture("iceland");
	tesTerrain->setPosition({ 0,20,10 });
	tesTerrain->setCamera(m_camera);
	AddEntity(tesTerrain);

	// Informaci�n de Debug
	Entity* debugTxt = new Entity("DebugText");
	debugTxt->setPosition({ 0,0,0 });
	debugTxt->addComponent(new DebugText(m_canvas, "panel"));
	AddEntity(debugTxt);
	debugTxt->setActive(true);

	/* - - GameManager - - */
	Entity* gm = new Entity("GameManager");
	// Componente GM
	GameManager* gmComponent = new GameManager(this, m_camera, botonesMenu, particleSys);
	gmComponent->setLights(dirLight, circleLight, spotLight, luzBlinn);
	gmComponent->setTessTerrain(tesTerrain);
	gmComponent->setParticleSys(particleSys);
	gm->addComponent(gmComponent);
	// Componente CameraController
	CameraController* camComp = new CameraController(m_camera);
	gm->addComponent(camComp);

	AddEntity(gm);

	/* - - - Efectos de postprocesado (el orden importa) - - - */
	//AddComposite((Shader*)&ResourceManager::Instance()->getComposite("byn"));
	//AddComposite((Shader*)&ResourceManager::Instance()->getComposite("waves"));
	//AddComposite((Shader*)&ResourceManager::Instance()->getComposite("bordes"));
	//AddComposite((Shader*)&ResourceManager::Instance()->getComposite("interference"));
}

void PruebaScene::loadResources()
{
	/* Mallas */
	ResourceManager::Instance()->loadMesh("Torre.obj", "torre");
	ResourceManager::Instance()->loadMesh("Peon.obj", "peon");
	ResourceManager::Instance()->loadMesh("star.obj", "star");
	ResourceManager::Instance()->loadMesh("redead.obj", "redead");

	/* Texturas */
	ResourceManager::Instance()->loadTexture("earth24.bmp", "earth");
	ResourceManager::Instance()->loadTexture("earthHD.jpg", "earthHD");
	ResourceManager::Instance()->loadTexture("venus.bmp", "venus");
	ResourceManager::Instance()->loadTexture("orientacion.bmp", "orientacion");
	ResourceManager::Instance()->loadTexture("Zelda.bmp", "zelda");
	ResourceManager::Instance()->loadTexture("terrenoTex.bmp", "terreno");
	ResourceManager::Instance()->loadTexture("iceland.bmp", "iceland", { 4, 65, 137 });
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
	// Los mapas de normales no pueden cargarse con SRGB_ALPHA
	ResourceManager::Instance()->loadTexture("brickwall_normal.png", "wall_normal", 255, GL_RGBA);
	ResourceManager::Instance()->loadTexture("star.png", "star");
	ResourceManager::Instance()->loadTexture("redead.png", "redead");
	ResourceManager::Instance()->loadTexture("windows.bmp", "windows");
	ResourceManager::Instance()->loadTexture("coke.jpg", "coke");

	// Canvas
	ResourceManager::Instance()->loadTexture("UI\\boton.png", "boton");
	ResourceManager::Instance()->loadTexture("UI\\panel.png", "panel");

	// Skyboxes - el orden tiene que ser este (top y bottom est�n invertidos por alguna raz�n)
	ResourceManager::Instance()->loadCubemapTexture({ "skyboxes/right.jpg", "skyboxes/left.jpg", "skyboxes/bottom.jpg",
		"skyboxes/top.jpg", "skyboxes/front.jpg", "skyboxes/back.jpg" }, "lakeSkybox");
	ResourceManager::Instance()->loadCubemapTexture({ "skyboxes/city/right.jpg", "skyboxes/city/left.jpg", "skyboxes/city/bottom.jpg",
		"skyboxes/city/top.jpg", "skyboxes/city/front.jpg", "skyboxes/city/back.jpg" }, "citySkybox");

	/* Materiales */
	ResourceManager::Instance()->loadMaterial("copper.material", "cobre");
	ResourceManager::Instance()->loadMaterial("crome.material", "cromo");
	ResourceManager::Instance()->loadMaterial("ruby.material", "ruby");
	ResourceManager::Instance()->loadMaterial("cristal.material", "cristal");
	ResourceManager::Instance()->loadMaterial("fluorescente.material", "fluorescente");
	ResourceManager::Instance()->loadMaterial("blinn.material", "blinn");
	ResourceManager::Instance()->loadMaterial("oceano.material", "oceano");

	// Prueba excepciones
	ResourceManager::Instance()->loadTexture("ladrillo.bmp", "ladrillo");
	ResourceManager::Instance()->loadMaterial("plata.material", "plata");

	/* Shaders */
	ResourceManager::Instance()->loadShader("default.vert", "cruces.geom", "default.frag", "cruces");
	ResourceManager::Instance()->loadShader("maximize.vert", "", "fog.frag", "bigFog");
	ResourceManager::Instance()->loadShader("default.vert", "", "movimiento.frag", "movimiento");
	ResourceManager::Instance()->loadShader("default.vert", "", "multitexture.frag", "multitexture");
	//ResourceManager::Instance()->loadShader("normalMap.vert", "", "normalMap.frag", "normalMap");
	ResourceManager::Instance()->loadShader("clippable.vert", "", "default.frag", "clippable");
	ResourceManager::Instance()->loadShader("terrain.vert", "terrain.tesc", "terrain.tese", "", "default.frag", "terreno");


	/* Efectos de postprocesado ('composites') */
	ResourceManager::Instance()->loadComposite("interference.frag", "interference");
	ResourceManager::Instance()->loadComposite("waves.frag", "waves");
	ResourceManager::Instance()->loadComposite("byn.frag", "byn");
	ResourceManager::Instance()->loadComposite("bordes.frag", "bordes");
}


void PruebaScene::PruebaMateriales()
{
	// Cubo con la misma textura en todas las caras
	Cubo* c = new Cubo(2);
	c->setTexture("caja");
	c->setPosition({ -10,0,0 });
	c->setShader("lights");
	AddEntity(c);

	// Cubo de orientaci�n (distintas texturas)
	Cubo* c2 = new Cubo(2, false);
	c2->setTexture("orientacion");
	c2->setMaterial("fluorescente");
	c2->setPosition({ -5,0,0 });
	AddEntity(c2);

	// Cubo default
	//Cubo* cuboDef = new Cubo(2);
	//cuboDef->setMaterial("default");
	//cuboDef->setPosition({ 0,0,0 });
	//AddEntity(cuboDef);

	// Cubo de cromo
	Cubo* cuboCromo = new Cubo(2);
	cuboCromo->setMaterial("cromo");
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
	tierra->addComponent(new RotationComp(1.0));
	tierra->setTexture("earth");
	tierra->setPosition({ 6,15,0 });
	tierra->rotate(-PI / 8, { 0, 0, 1 }, GLOBAL);
	tierra->setShader("cruces");
	AddEntity(tierra);

	// Rejilla (suelo)
	MovingGrid* grid = new MovingGrid(1, 1, 30, 60);
	grid->setTexture("agua");
	grid->setDisplacementMap("agua_disp");
	grid->setSpeeds({ 2.0, 0.0 }, { -2, 0 });
	//grid->setMaterial("cromo");
	grid->setPosition({ 0,-1,0 });
	AddEntity(grid);

	// "Cascada"
	MovingGrid* cascada = new MovingGrid(1, 1, 30, 24);
	cascada->setTexture("agua");
	cascada->setDisplacementMap("agua_disp");
	cascada->setSpeeds({ 5.0, 0.0 }, { 10.0, 0.0 });
	cascada->setPosition({ 30,-13,0 });
	cascada->rotate(-PI / 2, { 0, 0, 1 }, GLOBAL);
	AddEntity(cascada);

	// Terreno antiguo
	Terrain* oldTer = new Terrain();
	oldTer->loadRAW(ResourceManager::ASSETS_PATH + "terrain.raw", 0.05);
	oldTer->setTexture("terreno");
	oldTer->setPosition({ -30,0, -15 });
	AddEntity(oldTer);

	// Terreno Islandia
	Terrain* terrain = new Terrain();
	terrain->loadHeightMap(ResourceManager::TEXTURES_PATH + "iceland_height.bmp", 2.0);
	terrain->setTexture("iceland");
	//terrain->setShader("lights");
	terrain->setPosition({ 0,-20,0 });
	AddEntity(terrain);


	// Mar de la isla
	MovingGrid* mar = new MovingGrid(1, 1, 723, 1087);
	mar->setTexture("agua");
	mar->setMaterial("oceano");
	mar->setDisplacementMap("agua_disp");
	mar->setSpeeds({ 0.0, 0.0 }, { -0.2, -0.2 });
	//grid->setMaterial("cromo");
	mar->setPosition({ 0,-20.01,0 });
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
	NormalMapWall* pared = new NormalMapWall(1, 1, 15, 15);
	pared->setTexture("wall");
	pared->setNormalMap("wall_normal");
	pared->setMaterial("cromo");
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
	//pared2->addComponent(new RotationComp(0.5));
	AddEntity(pared2);

	//pared->rotate(-PI / 2, { 1,0,0 });
	//pared2->rotate(PI / 2, { 0,1,0 });


	// Plano para iluminaci�n Blinn-Phong
	Grid* plano = new Grid(1, 1, 30, 30);
	plano->setTexture("wall");
	plano->setMaterial("blinn");
	plano->setShader("lights");
	plano->setPosition({ 60,0,0 });
	AddEntity(plano);

	// Toro para clip planes
	ClippableEntity* cortado = new ClippableEntity();
	cortado->setTexture("default");
	cortado->setPosition({ -25,1,5 });
	AddEntity(cortado);

	// Esfera HD
	Esfera* esf = new Esfera(10, 30, 40);
	esf->setTexture("earthHD");
	esf->setPosition({ 0,10,25 });
	esf->setMaterial("cromo");
	esf->setShader("lights");
	esf->addComponent(new RotationComp(0.25));
	AddEntity(esf);
}

void PruebaScene::ViewportTest()
{
	Shader::turnOff();

	//Algunas variables locales para facilitar la puesta en escena
	Viewport* view = m_camera->getVP();
	GLdouble w = view->getW();
	GLdouble h = view->getH();
	// Reducimos el tama�o del puerto a 1/4 de la pantalla y dibujamos en los 3 primeros cuadrantes
	view->setSize(w / 2, h / 2);

	// Ejes RGB
	view->setPosition(0, h / 2);
	m_entities[0]->render(m_camera->getViewMat());

	// Pol�gono sin rellenar
	view->setPosition(w / 2, h / 2);
	m_entities[1]->render(m_camera->getViewMat());

	// Cubo texturizado
	view->setPosition(0, 0); // 0,0 es abajo a la izquierda
	m_entities[2]->render(m_camera->getViewMat());
	// Podemos dibujar un mismo objeto todas las veces que queramos (ejes extra)
	m_entities[0]->render(m_camera->getViewMat());

	// Cubo orientaci�n
	view->setPosition(w / 2, 0);
	m_entities[3]->render(m_camera->getViewMat());

	view->setSize(w, h); //Volvemos a dejar el viewPort como estaba
}
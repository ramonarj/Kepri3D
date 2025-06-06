#include "PruebaScene.h"

#include "../Kepri3D.h"

#include "Managers/GameManager.h"
#include "Components/DayCicle.h"
#include "Components/RotationComp.h"
#include "Managers/CameraController.h"
#include "Components/MultitexComp.h"
#include "Components/ParallaxComp.h"
#include "Components/AguaComp.h"
#include "Managers/DebugText.h"
#include "Callbacks.h"
#include "Components/Multiplayer.h"

const float buttonScale = 0.28f;
const float buttonMargin = 0.1f;
const float buttonSep = 0.12f;

std::vector<std::string> buttonNames = { "botonCulling", "botonBlending", "botonLighting", "botonTextures", "botonShading",
"botonAlpha", "botonMultisampling", "botonMipmaps", "botonNormales", "botonPostprocess", "botonScissor",
"botonSkybox", "botonGamma", "botonInstancing", "botonStencil", "botonLogicOp"};

std::vector<std::string> buttonTexts = { "CULLING", "BLENDING", "LIGHTING", "TEXTURES", "BOUNDING\nVOLUMES",
"ALPHA\nTEST", "MULTI\nSAMPLING", "MIPMAPS", "VER\nNORMALES", "POST\nPROCESADO", "SCISSOR\nTEST",
"SKYBOX", "FRESNEL\nEFFECT", "INSTANCING", "SOMBRAS", "LOGIC\nOP" };

std::vector<void(*)()> PruebaScene::callbacks = { cullingButtonPressed, blendingButtonPressed, lightingButtonPressed,
texturesButtonPressed, volumesButtonPressed, alphaButtonPressed, multisamplingButtonPressed, mipmapButtonPressed,
normalsButtonPressed, compositeButtonPressed, scissorButtonPressed, skyboxButtonPressed, fresnelButtonPressed, 
instancingButtonPressed, shadowsButtonPressed, logicOpButtonPressed, gammaButtonPressed };


void PruebaScene::setup()
{
	// LUCES

	// Puntual (va en c�rculos)
	Light* circleLight = new Light(POINT_LIGHT, { 1, 1, 0 });
	circleLight->setAmbient({ 0.1, 0.3, 0.1});
	circleLight->setSpecular({ 0.8, 0.8, 0});
	circleLight->emitShadows(true);
	Entity* circleLightEnt = new Esfera(0.5);
	((Material*)circleLightEnt->getMaterial())->setEmission(circleLight->getDiffuse()); //que la bolita sea del color de la luz
	circleLightEnt->addComponent(circleLight);
	AddEntity(circleLightEnt);

	// Foco (linterna)
	Light* spotLight = new Light(SPOT_LIGHT, { 0.7, 0.7, 0.7 });
	spotLight->setSpecular({ 0.6, 0.6, 0.6 });
	Entity* spotLightEnt = new Entity();
	spotLightEnt->addComponent(spotLight);
	AddEntity(spotLightEnt);

	// Nueva luz para el Blinn
	Light* luzBlinn = new Light(POINT_LIGHT, { 0.4, 0.4, 0.3 });
	luzBlinn->setAttenuationFactors(0.3, 0.3, 0.02);
	Entity* e = new Entity({ luzBlinn }, "LuzBlinn");
	e->setPosition({ 60, 4, -40 });
	AddEntity(e);

	// ENTIDADES
	// Ejes RGB
	//m_entities.push_back(new EjesRGB(0.5));

	// Pol�gono sin rellenar
	AddEntity(new Poligono(6, 0.5, false));

	// Pol�gono relleno
	Poligono* pol = new Poligono(4, 1, true);
	pol->setMaterial("zelda");
	AddEntity(pol);

	// Materiales y cubos
	PruebaMateriales();

	// Estrella del Mario 64
	Entity* estrella = new Entity("Estrella");
	estrella->setMesh("star");
	estrella->setMaterial("star");
	estrella->setPosition({ -5,2,-6 });
	AddEntity(estrella);
	estrella->addComponent(new RotationComp(1.0));

	// Redead del Zelda OoT
	Entity* redead = new Entity("Redead");
	redead->setMesh("redead");
	redead->setMaterial("redead");
	redead->getRenderer()->setMaterial(1, "star");
	redead->getRenderer()->setMaterial(2, "oceano");
	redead->setPosition({ -10,2,-6 });
	redead->rotate(PI / 4, { 0, 0, 1 });
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
	torre->setMaterial("ruby");
	torre->setPosition({ 0,15,-15 });
	torre->addComponent(new RotationComp(0.5));
	AddEntity(torre);
	// Para que el blending del rub� funcione bien (de momento), 
	// tiene que estar la �ltima de las entidades

	// Esferita hija de la torre
	Esfera* esferita = new Esfera(1.0, 5, 10);
	esferita->setMaterial("earth");
	esferita->setPosition({ 0,2,-6 });
	esferita->addComponent(new RotationComp(1));
	esferita->setParent(torre);
	//AddEntity(esferita);

	// Un toro
	Toro* toro = new Toro(0.5, 0.25, 20, 10);
	toro->setPosition({ 2,0,-2 });
	toro->setMaterial("fluorescente");
	toro->setParent(esferita);
	//AddEntity(toro);

	// Un cilindro
	Cilindro* cilindro = new Cilindro(2, 4);
	cilindro->setPosition({ 24,-0.9,-10 });
	cilindro->setMaterial("coke");
	AddEntity(cilindro);

	// Un cono
	Cono* cono = new Cono(2, 5, 30);
	cono->setPosition({ 24,-0.95,10 });
	cono->setMaterial("cono");
	AddEntity(cono);

	// Prueba vertex painting (degradado verde-azul)
	int numVerts = cono->getRenderer()->getMesh()->getVerticesNum();
	glm::dvec4* colores = cono->getRenderer()->getMesh()->getColors();
	for (int i = 0; i < numVerts; i++)
	{
		colores[i] = Kepri::lerp({ 0, 1, 0, 1 }, { 0, 0, 1, 1 }, i / (float)numVerts);
	}

	// Un cubo multitextura
	Cubo* cuboMT = new Cubo(2);
	cuboMT->setName("CuboMT");
	cuboMT->setMaterial("lego");
	cuboMT->setPosition({ -15, 0, 6 });
	cuboMT->getRenderer()->castShadows(false);
	cuboMT->addComponent(new MultitexComp((Material*)cuboMT->getMaterial()));
	AddEntity(cuboMT);

	// Una caja con mapa especular
	Cubo* cuboSpec = new Cubo(2);
	cuboSpec->setName("CuboSpecmap");
	cuboSpec->setMaterial("specularCube");
	cuboSpec->setPosition({ 0, 0, 5 });
	AddEntity(cuboSpec);

	// Hierba
	Hierba* hierba = new Hierba(2.5, 3);
	hierba->getMaterial()->setTexture(DIFFUSE_MAP, ResourceManager::Instance()->getTexture("hierba"));
	hierba->setPosition({ 15, 0.5, 5 });
	hierba->rotate(-PI / 2, { 0, 1, 0 }, GLOBAL);
	hierba->addComponent(new RotationComp(0.75));
	AddEntity(hierba);


	// Matojos de hierba (billboards). TODO mejorar esto
	Billboard* matojos = new Billboard("Matojos", 0, 0);
	matojos->setPosition({ 50, -9, 0 });
	int numMatojos = 20;
	float radioMax = 15.0f;
	for(int i = 0; i < numMatojos; i++)
	{
		glm::vec3 incr = { (std::rand() % 100) / (100 / radioMax), 0, (std::rand() % 100) / (100 / radioMax) };
		Billboard* bill = new Billboard("hierba", 2, 2.5);
		bill->setPosition(incr);
		bill->setParent(matojos);
		//AddEntity(bill);
	}
	AddEntity(matojos);

	// �rbol (billboard)
	Billboard* tree = new Billboard("tree", 8, 10);
	tree->setPosition({ 85, -0.3, 57 });
	tree->setAncla({ 0, -4.25 });
	AddEntity(tree);

	/* - - Canvas - - */
	m_canvas = new Canvas();

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
		Button* b = new Button(125, 60, m_canvas);
		b->setName(buttonNames[i]);
		glm::vec2 pos;
		if (i < 8)
			pos = { buttonMargin, 1 - buttonMargin - i * buttonSep };
		else
			pos = { 1 - buttonMargin, 1 - buttonMargin - (i - 8) * buttonSep };
		b->setPositionUI(pos.x, pos.y);
		//b->setScaleUI(buttonScale, buttonScale);
		b->setCallback(callbacks[i]);
		b->setTexture("boton");
		b->setHoverTexture("botonHover");
		b->setParent(botonesMenu); // hijos del nodo vac�o

		// El texto que contiene
		Text* t = new Text(buttonTexts[i], m_canvas, {1, 1, 1, 1});
		t->setPositionUI(0.43, 0.5); // alineado a la izquierda
		t->setScaleUI(0.7, 0.7);
		t->setGrosor(1.5);
		t->setParent(b);
	}


	/* - - Skybox - - */
	//Skybox* sky = new Skybox("lakeSkybox");
	//SetSkybox(sky);

	/* - - Sistema de part�culas - - */
	// Con 10.000, empieza a ir demasiado lento
	ParticleSystem* particleSys = new ParticleSystem(0.4, 1000, SPHERE, PARTICLE_2D);
	particleSys->setTexture("emoji");
	particleSys->setParticleSpeed(3.0);
	particleSys->setLifetime(2);
	particleSys->setActive(false);
	AddEntity(particleSys);

	// Terreno teselado
	TessTerrain* tesTerrain = new TessTerrain(18, 28, 80, 80); //patches de 80x80uds
	tesTerrain->setMaterial("iceland");
	tesTerrain->setPosition({ 0,-20,0 });
	tesTerrain->getRenderer()->setPolygonMode(GL_FILL, GL_LINE);
	Terreno* terrainComp = new Terreno((Material*)tesTerrain->getMaterial());
	terrainComp->setCamera(m_camera);
	terrainComp->setHeightMap("iceland_height", 25.0f);
	tesTerrain->addComponent(terrainComp);
	AddEntity(tesTerrain);

	// Informaci�n de Debug
	Entity* debugTxt = new Entity("DebugText");
	debugTxt->setPosition({ 0,0,0 });
	debugTxt->addComponent(new DebugText(m_canvas, "panel"));
	AddEntity(debugTxt);
	debugTxt->setActive(true);

	/* - - GameManager - - */
	// Componente GM
	GameManager* gmComponent = new GameManager(this, m_camera, botonesMenu, particleSys);
	gmComponent->setLights(circleLight, spotLight, luzBlinn);
	gmComponent->setTessTerrain(terrainComp);
	gmComponent->setParticleSys(particleSys);
	// Componente CameraController
	CameraController* camComp = new CameraController(m_camera);
	// Componente DayCicle
	DayCicle* daynightComp = new DayCicle(1); // crea el cielo y el sol
	// Componente Multiplayer
	//Multiplayer* multiplayerComp = new Multiplayer(8);
	Entity* gm = new Entity({ gmComponent, camComp, daynightComp }, "GameManager");
	AddEntity(gm);


	/* - - - Efectos de postprocesado (el orden importa) - - - */
	AddComposite(ResourceManager::Instance()->getComposite("byn"));
	//AddComposite(ResourceManager::Instance()->getComposite("waves"));
	//AddComposite(ResourceManager::Instance()->getComposite("bordes"));
	//AddComposite(ResourceManager::Instance()->getComposite("interference"));
	AddComposite(ResourceManager::Instance()->getComposite("fog"));
}

void PruebaScene::loadResources()
{
	/* Mallas */
	ResourceManager::Instance()->loadMesh("Torre.obj", "torre");
	ResourceManager::Instance()->loadMesh("Peon.obj", "peon");
	ResourceManager::Instance()->loadMesh("star.obj", "star");
	ResourceManager::Instance()->loadMesh("redead.obj", "redead", 0.1f);

	/* Texturas (IMPORTANTE cargar antes que los materiales) */
	ResourceManager::Instance()->loadTexture("earth24.bmp", "earth");
	ResourceManager::Instance()->loadTexture("earthHD.jpg", "earthHD");
	ResourceManager::Instance()->loadTexture("venus.bmp", "venus");
	ResourceManager::Instance()->loadTexture("orientacion.bmp", "orientacion");
	ResourceManager::Instance()->loadTexture("orientacion_emission.png", "orientacion_emission");
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
	ResourceManager::Instance()->loadTexture("brickwall_disp2.png", "wall_disp", 255, GL_RGBA);
	ResourceManager::Instance()->loadTexture("star.png", "star");
	ResourceManager::Instance()->loadTexture("redead.png", "redead");
	ResourceManager::Instance()->loadTexture("redead_emission.png", "redead_emission");
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
	ResourceManager::Instance()->loadTexture("UI\\botonHover.png", "botonHover");
	ResourceManager::Instance()->loadTexture("UI\\panel.png", "panel");

	// Skyboxes - el orden tiene que ser este (top y bottom est�n invertidos por alguna raz�n)
	ResourceManager::Instance()->loadCubemapTexture({ "skyboxes/right.jpg", "skyboxes/left.jpg", "skyboxes/bottom.jpg",
		"skyboxes/top.jpg", "skyboxes/front.jpg", "skyboxes/back.jpg" }, "lakeSkybox");
	ResourceManager::Instance()->loadCubemapTexture({ "skyboxes/city/right.jpg", "skyboxes/city/left.jpg", "skyboxes/city/bottom.jpg",
		"skyboxes/city/top.jpg", "skyboxes/city/front.jpg", "skyboxes/city/back.jpg" }, "citySkybox");

	/* Shaders (ES IMPORTANTE CARGARLOS ANTES QUE LOS MATERIALES) */
	ResourceManager::Instance()->loadShader("default.vert", "cruces.geom", "default.frag", "cruces");
	ResourceManager::Instance()->loadShader("maximize.vert", "", "default.frag", "big");
	ResourceManager::Instance()->loadShader("default.vert", "", "multitexture.frag", "multitexture");
	ResourceManager::Instance()->loadShader("clippable.vert", "", "default.frag", "clippable");
	ResourceManager::Instance()->loadShader("terrain.vert", "terrain.tesc", "terrain.tese", "", "lights.frag", "terreno");
	ResourceManager::Instance()->loadShader("reflejos.vert", "", "reflejos.frag", "reflejos");
	ResourceManager::Instance()->loadShader("vertex_paint.vert", "", "vertex_paint.frag", "vertexPaint");

	/* Materiales */
	ResourceManager::Instance()->loadMaterial("pared.material", "pared");
	ResourceManager::Instance()->loadMaterial("paredParallax.material", "paredParallax");
	ResourceManager::Instance()->loadMaterial("copper.material", "cobre");
	ResourceManager::Instance()->loadMaterial("crome.material", "cromo");
	ResourceManager::Instance()->loadMaterial("ruby.material", "ruby");
	ResourceManager::Instance()->loadMaterial("cristal.material", "cristal");
	ResourceManager::Instance()->loadMaterial("fluorescente.material", "fluorescente");
	ResourceManager::Instance()->loadMaterial("blinn.material", "blinn");
	ResourceManager::Instance()->loadMaterial("oceano.material", "oceano", "agua");
	ResourceManager::Instance()->loadMaterial("emerald.material", "emerald", "flat");
	ResourceManager::Instance()->loadMaterial("specularCube.material", "specularCube");
	ResourceManager::Instance()->loadMaterial("iceland.material", "iceland", "terreno");
	ResourceManager::Instance()->loadMaterial("redead.material", "redead");
	ResourceManager::Instance()->loadMaterial("orientacion.material", "orientacion");
	ResourceManager::Instance()->loadMaterial("earth.material", "earth", "cruces");
	ResourceManager::Instance()->loadMaterial("coke.material", "coke");
	ResourceManager::Instance()->loadMaterial("zelda.material", "zelda");
	ResourceManager::Instance()->loadMaterial("caja.material", "caja");
	ResourceManager::Instance()->loadMaterial("star.material", "star", "flat");
	ResourceManager::Instance()->loadMaterial("espejo.material", "espejo", "reflejos");
	ResourceManager::Instance()->loadMaterial("reflejos.material", "reflejos", "reflejos");
	ResourceManager::Instance()->loadMaterial("lego.material", "lego", "multitexture");
	ResourceManager::Instance()->loadMaterial("cono.material", "cono", "vertexPaint");

	// Prueba excepciones
	ResourceManager::Instance()->loadTexture("ladrillo.bmp", "ladrillo");
	ResourceManager::Instance()->loadMaterial("plata.material", "plata");

	/* Efectos de postprocesado ('composites') */
	ResourceManager::Instance()->loadComposite("interference.frag", "interference");
	ResourceManager::Instance()->loadComposite("waves.frag", "waves");
	ResourceManager::Instance()->loadComposite("byn.frag", "byn");
	ResourceManager::Instance()->loadComposite("bordes.frag", "bordes");
	ResourceManager::Instance()->loadComposite("fog.frag", "fog");
}


void PruebaScene::PruebaMateriales()
{
	// Cubo de orientaci�n (distintas texturas)
	Cubo* c2 = new Cubo(2, false);
	c2->setMaterial("orientacion");
	c2->setPosition({ -5,0,0 });
	AddEntity(c2);

	// Cubo con la misma textura en todas las caras
	Cubo* c = new Cubo(2);
	c->setMaterial("caja");
	c->setPosition({ -10,0,0 });
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
	cuboCobre->setMaterial("cobre");
	cuboCobre->setPosition({ 10,0,0 });
	AddEntity(cuboCobre);

	// Esfera
	Esfera* esfera = new Esfera(1, 8, 16);
	esfera->setMaterial("earth");
	esfera->setPosition({ -15,2,-3 });
	//esfera->setShader("big");
	AddEntity(esfera);

	// Tierra
	Esfera* tierra = new Esfera(3);
	tierra->setName("TierraCruces");
	tierra->setMaterial("earth");
	tierra->setPosition({ 6,15,0 });
	tierra->rotate(-PI / 8, { 0, 0, 1 }, GLOBAL);
	tierra->addComponent(new RotationComp(1.0));
	AddEntity(tierra);

	// Rejilla (suelo)
	Grid* grid = new Grid(1, 1, 30, 60);
	grid->setMaterial("oceano");
	grid->setPosition({ 0,-1,0 });
	grid->getRenderer()->castShadows(false);
	AguaComp* sueloComp = new AguaComp((Material*)grid->getMaterial());
	sueloComp->setSpeeds({ 2.0, 0.0 }, { -2, 0 });
	grid->addComponent(sueloComp);
	AddEntity(grid);

	// "Cascada"
	Grid* cascada = new Grid(1, 1, 30, 24);
	cascada->setName("Cascada");
	cascada->setMaterial("oceano");
	cascada->setPosition({ 30,-13,0 });
	cascada->rotate(-PI / 2, { 0, 0, 1 }, GLOBAL);
	AguaComp* cascadaComp = new AguaComp((Material*)cascada->getMaterial());
	cascadaComp->setSpeeds({ 5.0, 0.0 }, { 10.0, 0.0 }); 
	cascada->addComponent(cascadaComp);
	AddEntity(cascada);

	// Terreno antiguo
	//Terrain* oldTer = new Terrain();
	//oldTer->loadRAW(ResourceManager::ASSETS_PATH + "terrain.raw", 0.05);
	//oldTer->setPosition({ -30,0, -15 });
	//AddEntity(oldTer);

	// Terreno Islandia
	//Terrain* terrain = new Terrain();
	//terrain->loadHeightMap(ResourceManager::TEXTURES_PATH + "iceland_height.bmp", 2.0);
	//terrain->setPosition({ 0,-20,0 });
	//AddEntity(terrain);


	// Mar de la isla
	Grid* mar = new Grid(1, 1, 723 * 3, 1087 * 3);
	mar->setMaterial("oceano");
	mar->setPosition({ 0,-20.01,0 });
	AguaComp* marComp = new AguaComp((Material*)mar->getMaterial());
	marComp->setSpeeds({ 0.0, 0.0 }, { -0.2, -0.2 });
	mar->addComponent(marComp);
	AddEntity(mar);

	// Minimap
	//Terrain* miniTer = new Terrain();
	//miniTer->loadHeightMap("../../bin/assets/icelandHeight.bmp", 0.01);
	//miniTer->scale({ 1,1.5,1 });
	//miniTer->setPosition({ -6,-0.99,6 });
	//miniTer->rotate(PI / 2, { 0,1,0 }, GLOBAL);
	//AddEntity(miniTer);

	// Pared de ladrillos (parallax maps)
	Grid* paredParallax = new Grid(1, 1, 15, 15);
	paredParallax->setName("ParallaxWall");
	paredParallax->setMaterial("paredParallax");
	paredParallax->setPosition({ -20,6.5,-15 });
	paredParallax->rotate(PI / 2, { 1,0,0 });
	//paredParallax->addComponent(new RotationComp(0.5));
	paredParallax->addComponent(new ParallaxComp((Material*)paredParallax->getMaterial()));
	AddEntity(paredParallax);

	// Pared de ladrillos (normal maps)
	Grid* pared = new Grid(1, 1, 15, 15);
	pared->setName("NormalMapWall");
	pared->setMaterial("pared");
	pared->setPosition({ -5,6.5,-15 });
	pared->rotate(PI / 2, { 1,0,0 });
	//pared->addComponent(new RotationComp(0.5));
	AddEntity(pared);

	// Pared sin normal map para comparar
	Grid* pared2 = new Grid(1, 1, 15, 15);
	pared2->setMaterial("pared");
	pared2->getMaterial()->setTexture(NORMAL_MAP, nullptr); //quitarle el normal map
	pared2->setPosition({ 10,6.5,-15 });
	pared2->rotate(PI / 2, { 1,0,0 });
	//pared2->receiveShadows(false);
	//pared2->addComponent(new RotationComp(0.5));
	AddEntity(pared2);

	// Plano para iluminaci�n Blinn-Phong
	Grid* plano = new Grid(1, 1, 30, 30);
	plano->setMaterial("blinn");
	plano->setPosition({ 60,0,-40 });
	AddEntity(plano);

	// Toro para clip planes
	ClippableEntity* cortado = new ClippableEntity();
	cortado->setPosition({ -25,1,5 });
	AddEntity(cortado);

	// Esfera HD
	Esfera* esf = new Esfera(10, 30, 40);
	esf->setPosition({ 0,10,25 });
	esf->setMaterial("emerald");
	esf->addComponent(new RotationComp(0.25));
	esf->addComponent(new Collider(10));
	esf->addComponent(new Rigid(esf->getModelMat(), Static));
	AddEntity(esf);


	// Cubo con reflejos
	Cubo* cuboReflejos = new Cubo(6.0);
	cuboReflejos->setMaterial("reflejos");
	cuboReflejos->setPosition({ 40,3,0 });
	cuboReflejos->addComponent(new RotationComp(0.5));
	AddEntity(cuboReflejos);

	// Esfera con reflejos
	Esfera* esfReflejos = new Esfera(6, 30, 40);
	esfReflejos->setMaterial("reflejos");
	//esfReflejos->getMaterial()->setTexture(SKYBOX_MAP, ResourceManager::Instance()->getTexture("citySkybox"));
	esfReflejos->setPosition({ 40,25,0 });
	AddEntity(esfReflejos);

	// Espejo
	Grid* espejo = new Grid(1, 1, 4, 2);
	espejo->setName("espejo");
	espejo->setMaterial("espejo");
	espejo->setPosition({ 25,3,0 });
	espejo->rotate(PI / 2.0, { 1, 0, 0 });
	AddEntity(espejo);

	// Prueba para sombras
	Grid* tejado = new Grid(1, 1, 15, 15);
	tejado->setMaterial("cobre");
	tejado->setPosition({ 0,10,0 });
	AddEntity(tejado);

	// Entidad con VBO
	//VBOEntity* vboEnt = new VBOEntity();
	//vboEnt->setMaterial("default");
	//vboEnt->setPosition({ 0,1,-5 });
	//AddEntity(vboEnt);

	// Entidad con EBO
	EBOEntity* eboEnt = new EBOEntity();
	eboEnt->setMaterial("default");
	eboEnt->setPosition({ 5,1,-5 });
	AddEntity(eboEnt);

	// Entidad con VAO
	VAOEntity* vaoEnt = new VAOEntity();
	vaoEnt->setMaterial("default");
	vaoEnt->setPosition({ 10,1,-5 });
	AddEntity(vaoEnt);

	// Prueba Sponza
	Entity* sponza = new Entity("Sponza");
	sponza->setMesh("sponza1");
	sponza->setMaterial("cobre");
	AddEntity(sponza);

	// Muchas entidades para ver los efectos del Frustum culling
	for(int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			Entity* arbol = new Cilindro(2.0, 10.0);
			arbol->setPosition({ -300 + i * 20, 20, -300 + j * 20 });
			AddEntity(arbol);
		}
	}
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
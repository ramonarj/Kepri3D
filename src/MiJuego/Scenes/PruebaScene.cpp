#include "PruebaScene.h"

#include "../Kepri3D.h"

#include "../GameManager.h"
#include "../RotationComp.h"
#include "../CameraController.h"

const float buttonScale = 0.27f;
const float buttonMargin = 0.1f;
const float buttonSep = 0.12f;

std::vector<std::string> buttonNames = { "botonCulling", "botonBlending", "botonLighting", "botonTextures", "botonShading",
"botonAlpha", "botonMultisampling", "botonMipmaps", "botonNormales", "botonPostprocess", "botonScissor",
"botonSkybox", "botonGamma", "botonInstancing", "botonStencil", "botonLogicOp"};

std::vector<void(*)()> PruebaScene::callbacks = { cullingButtonPressed, blendingButtonPressed, lightingButtonPressed,
texturesButtonPressed, shadingButtonPressed, alphaButtonPressed, multisamplingButtonPressed, mipmapButtonPressed,
normalsButtonPressed, compositeButtonPressed, scissorButtonPressed, skyboxButtonPressed, gammaButtonPressed, 
instancingButtonPressed, stencilButtonPressed, logicOpButtonPressed };

ParticleSystem* PruebaScene::particleSys = nullptr;

void PruebaScene::init()
{
	// LUCES
	// Direccional
	Light* dirLight = new Light(DIRECTIONAL_LIGHT);
	dirLight->setDirection({ -1,0 , 0 });
	dirLight->setDiffuse({ 1, 1, 1, 1.0 });
	dirLight->setAmbient({ 0.2, 0.2, 0.5, 0.0 });
	dirLight->setActive(true);
	AddLight(dirLight);

	// Puntual (va en círculos)
	Light* circleLight = new Light(POINT_LIGHT, { 1, 1, 0, 1 });
	circleLight->setPosition({ 3,3,-3 });
	circleLight->setAmbient({ 0.1, 0.5, 0.1, 1.0 });
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
	// Para que el blending del rubí funcione bien (de momento), 
	// tiene que estar la última de las entidades

	// Esferita hija de la torre
	Esfera* esferita = new Esfera(0.5, 10, true);
	//esferita->setTexture("earth");
	esferita->setPosition({ 0,3.5,-2 });
	esferita->setParent(torre);
	//AddEntity(esferita);

	// Un toro
	Toro* toro = new Toro(0.75, 0.4, 30, 10);
	toro->setPosition({ 20,-0.4,6 });
	toro->setMaterial("fluorescente");
	AddEntity(toro);

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

	
	// Entidad vacía para que cuelguen los botones
	UIElement* botonesMenu = new UIElement();
	botonesMenu->setActive(false);
	m_canvas->addElement(botonesMenu);

	// Botones
	for (int i = 0; i < buttonNames.size(); i++)
	{
		// Su textura, posición, escala y callback
		Button* b = new Button(buttonNames[i], m_canvas);
		if (i < 8)
			b->setPositionUI(buttonMargin, 0.92 - i * buttonSep);
		else
			b->setPositionUI(1 - buttonMargin, 0.92 - (i - 8) * buttonSep);
		b->setScaleUI(buttonScale, buttonScale);
		b->setCallback(callbacks[i]);
		b->setParent(botonesMenu); // hijos del nodo vacío
	}

	// Textos de prueba
	//ABCDEFGHIJKLMNOPQRSTUVWXYZ
	Text* texto = new Text("EN UN LUGAR DE LA MANCHA", m_canvas, {0, 1, 0, 1});
	texto->setPositionUI(0.15, 0.1);
	m_canvas->addElement(texto);

	Text* textoNums = new Text("0123456789-/", m_canvas, { 0, 1, 0, 1 });
	textoNums->setPositionUI(0.3, 0.05);
	textoNums->setGrosor(2.0);
	m_canvas->addElement(textoNums);


	/* - - Skybox - - */
	Skybox* sky = new Skybox("citySkybox");
	SetSkybox(sky);

	/* - - Sistema de partículas - - */
	// Con 10.000, empieza a ir demasiado lento
	particleSys = new ParticleSystem(0.4, 1000, PARTICLE_2D);
	particleSys->setTexture("emoji");
	particleSys->setParticleSpeed(3.0);
	particleSys->setLifetime(2);
	particleSys->setActive(false);
	AddEntity(particleSys);

	// Terreno teselado
	TessTerrain* tesTerrain = new TessTerrain(2, 2, 3, 3);
	tesTerrain->setTexture("windows");
	tesTerrain->setPosition({ 0,10,10 });
	AddEntity(tesTerrain);

	/* - - GameManager - - */
	Entity* gm = new Entity("GameManager");
	// Componente GM
	GameManager* gmComponent = new GameManager(this, m_camera, botonesMenu, particleSys);
	gmComponent->setLights(dirLight, circleLight, spotLight, luzBlinn);
	gmComponent->setTessTerrain(tesTerrain);
	gm->addComponent(gmComponent);
	// Componente CameraController
	CameraController* camComp = new CameraController(m_camera);
	gm->addComponent(camComp);

	AddEntity(gm);

	/* - - - Efectos de postprocesado (el orden importa) - - - */
	//AddComposite((Shader*)&ResourceManager::Instance()->getComposite("byn"));
	//AddComposite((Shader*)&ResourceManager::Instance()->getComposite("waves"));
	AddComposite((Shader*)&ResourceManager::Instance()->getComposite("bordes"));
	AddComposite((Shader*)&ResourceManager::Instance()->getComposite("interference"));
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
	ResourceManager::Instance()->loadTexture("brickwall_normal.png", "wall_normal");
	ResourceManager::Instance()->loadTexture("star.png", "star");
	ResourceManager::Instance()->loadTexture("redead.png", "redead");
	ResourceManager::Instance()->loadTexture("windows.bmp", "windows");

	// Botones
	for (int i = 0; i < buttonNames.size(); i++)
	{
		ResourceManager::Instance()->loadTexture("UI\\" + buttonNames[i] + ".bmp", buttonNames[i]);
	}

	// Skyboxes - el orden tiene que ser este (top y bottom están invertidos por alguna razón)
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
	ResourceManager::Instance()->loadShader("normalMap.vert", "", "normalMap.frag", "normalMap");
	ResourceManager::Instance()->loadShader("clippable.vert", "", "default.frag", "clippable");
	ResourceManager::Instance()->loadShader("default.vert", "terrain.tesc", "terrain.tese", "", "default.frag", "terreno");


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

	// Cubo de orientación (distintas texturas)
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
	Esfera* esfera = new Esfera(1, 8, true);
	esfera->setTexture("earth");
	esfera->setPosition({ -15,2,-3 });
	esfera->setShader("bigFog");
	AddEntity(esfera);

	// Tierra
	Esfera* tierra = new Esfera(3, 20, true);
	tierra->addComponent(new RotationComp(1.0));
	tierra->setTexture("earth");
	tierra->setPosition({ 6,15,0 });
	tierra->rotate(-PI / 8, { 0, 0, 1 }, GLOBAL);
	tierra->setShader("cruces");
	AddEntity(tierra);

	// Rejilla (suelo)
	MovingGrid* grid = new MovingGrid(10, 20, 3, 3);
	grid->setTexture("agua");
	grid->setDisplacementMap("agua_disp");
	grid->setSpeeds({ 2.0, 0.0 }, { -2, 0 });
	//grid->setMaterial("cromo");
	grid->setPosition({ 0,-1,0 });
	AddEntity(grid);

	// "Cascada"
	MovingGrid* cascada = new MovingGrid(10, 8, 3, 3);
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
	MovingGrid* mar = new MovingGrid(1, 1, 1087, 723);
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
	pared->addComponent(new RotationComp(0.5));
	AddEntity(pared);

	// Pared sin normal map para comparar
	Grid* pared2 = new Grid(1, 1, 15, 15);
	pared2->setTexture("wall");
	pared2->setShader("lights");
	pared2->setPosition({ 10,6.5,-15 });
	pared2->rotate(PI / 2, { 1,0,0 });
	pared2->addComponent(new RotationComp(0.5));
	AddEntity(pared2);

	//pared->rotate(-PI / 2, { 1,0,0 });
	//pared2->rotate(PI / 2, { 0,1,0 });


	// Plano para iluminación Blinn-Phong
	Grid* plano = new Grid(10, 10, 3, 3);
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
	Esfera* esf = new Esfera(10, 40);
	esf->setTexture("default");
	esf->setPosition({ 0,10,25 });
	esf->setMaterial("cromo");
	esf->setShader("lights");
	AddEntity(esf);
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

void PruebaScene::centerMouse()
{
	InputManager::Instance()->setMousePos(400, 300);
}

// - - - - - - - - - - - - Callbacks - - - - - - - - - - - - - - - //

void PruebaScene::cullingButtonPressed()
{
	// Activar / desactivar el culling de polígonos traseros
	Game::switchBoolParam(GL_CULL_FACE);
	centerMouse();
}

void PruebaScene::blendingButtonPressed()
{
	// Activar / desactivar las transparencias en texturas y materiales
	Game::switchBoolParam(GL_BLEND);
	centerMouse();
}

void PruebaScene::lightingButtonPressed()
{
	// Activar / desactivar la iluminación
	Game::switchBoolParam(GL_LIGHTING);
	centerMouse();
}

void PruebaScene::texturesButtonPressed()
{
	// Activar / desactivar el uso de texturas
	Game::switchBoolParam(GL_TEXTURE_2D);
	centerMouse();
}

void PruebaScene::shadingButtonPressed()
{
	// Cambiar entre sombreado FLAT y SMOOTH
	GLint shadeType;
	glGetIntegerv(GL_SHADE_MODEL, &shadeType);
	if (shadeType == GL_FLAT)
	{
		glShadeModel(GL_SMOOTH);
		Material::setShadingType(GL_SMOOTH);
	}

	else
	{
		glShadeModel(GL_FLAT);
		Material::setShadingType(GL_FLAT);
	}

	centerMouse();
}

void PruebaScene::alphaButtonPressed()
{
	// Activar / desactivar el alpha test
	Game::switchBoolParam(GL_ALPHA_TEST);
	centerMouse();
}

void PruebaScene::multisamplingButtonPressed()
{
	// Activar / desactivar el multisampling
	Game::switchBoolParam(GL_MULTISAMPLE);
	centerMouse();
}

void PruebaScene::mipmapButtonPressed()
{
	// Activar / desactivar el uso de mipmaps
	mipmapsActive = !mipmapsActive;
	ResourceManager::Instance()->enableMipmaps(mipmapsActive);

	centerMouse();
}

void PruebaScene::normalsButtonPressed()
{
	// Activar / desactivar la visualización de vectores normales a cada vértice
	if (normalsShader == nullptr)
		normalsShader = (Shader*)&ResourceManager::Instance()->getShader("normals");
	else
		normalsShader = nullptr;

	centerMouse();
}

void PruebaScene::compositeButtonPressed()
{
	// Quitar todos los efectos de composite menos el predet.
	while (m_composites.size() > 1)
		m_composites.pop_back();

	centerMouse();
}


void PruebaScene::scissorButtonPressed()
{
	int proporcionBarra = glutGet(GLUT_WINDOW_HEIGHT) / 6.0f;
	// Actualizar el área visible
	Game::updateScissorBox(0, proporcionBarra, glutGet(GLUT_WINDOW_WIDTH), 
		glutGet(GLUT_WINDOW_HEIGHT) - proporcionBarra * 2);
	// Activar / desactivar el Scissor Test (hay que hacerlo en este orden)
	Game::switchBoolParam(GL_SCISSOR_TEST);

	centerMouse();
}

void PruebaScene::skyboxButtonPressed()
{
	skyboxActive = !skyboxActive;
	centerMouse();
}

void PruebaScene::gammaButtonPressed()
{
	Game::switchBoolParam(GL_FRAMEBUFFER_SRGB);
	centerMouse();
}

void PruebaScene::stencilButtonPressed()
{
	Game::switchBoolParam(GL_STENCIL_TEST);
	centerMouse();
}

void PruebaScene::logicOpButtonPressed()
{
	// Activar las operaciones lógicas implica desactivar totalmente el 'Blending'
	Game::switchBoolParam(GL_COLOR_LOGIC_OP);

	// Tipo de operación que se hace sobre cada píxel
	// Curiosas : GL_COPY (predet.), GL_COPY_INVERTED (negativo), GL_INVERT (b/n)
	glLogicOp(GL_COPY_INVERTED);

	centerMouse();
}


void PruebaScene::instancingButtonPressed() 
{
	if(particleSys != nullptr)
	{
		particleSys->setActive(!particleSys->isActive());
	}
	centerMouse();
}

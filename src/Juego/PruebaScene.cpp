#include "PruebaScene.h"

#include "../Kepri3D.h"

#include "GameManager.h"

const float buttonScale = 0.28f;
const float buttonSep = 0.13f;

std::vector<std::string> buttonNames = { "botonCulling", "botonBlending", "botonLighting", "botonTextures", "botonShading",
"botonAlpha", "botonMultisampling", "botonMipmaps", "botonNormales", "botonPostprocess", "botonScissor", "botonSkybox", "botonGamma" };

std::vector<void(*)()> PruebaScene::callbacks = { cullingButtonPressed, blendingButtonPressed, lightingButtonPressed,
texturesButtonPressed, shadingButtonPressed, alphaButtonPressed, multisamplingButtonPressed, mipmapButtonPressed,
normalsButtonPressed, compositeButtonPressed, scissorButtonPressed, skyboxButtonPressed, gammaButtonPressed };

void PruebaScene::init()
{
	// Hacer que el cursor sea invisible y moverlo al centro de la ventana
	glutSetCursor(GLUT_CURSOR_NONE);
	glutWarpPointer(m_camera->getVP()->getW() / 2, m_camera->getVP()->getH() / 2);

	// CARGA DE RECURSOS PARA ESTA ESCENA
	/* Mallas que vamos a usar */
	ResourceManager::Instance()->loadMesh("Torre.obj", "torre");
	ResourceManager::Instance()->loadMesh("Peon.obj", "peon");

	/* Texturas que vamos a usar */
	ResourceManager::Instance()->loadTexture("earth24.bmp", "earth");
	ResourceManager::Instance()->loadTexture("venus.bmp", "venus");
	ResourceManager::Instance()->loadTexture("orientacion.bmp", "orientacion");
	ResourceManager::Instance()->loadTexture("Zelda.bmp", "zelda");
	ResourceManager::Instance()->loadTexture("terrenoTex.bmp", "terreno");
	ResourceManager::Instance()->loadTexture("caja.bmp", "caja");
	ResourceManager::Instance()->loadTexture("caja_specular.bmp", "caja_spec");
	ResourceManager::Instance()->loadTexture("cobre.bmp", "cobre");
	ResourceManager::Instance()->loadTexture("agua.bmp", "agua");

	// Botones
	for (int i = 0; i < buttonNames.size(); i++)
	{
		ResourceManager::Instance()->loadTexture(buttonNames[i] + ".bmp", buttonNames[i]);
	}

	/* Materiales que vamos a usar */
	ResourceManager::Instance()->loadMaterial("copper.material", "cobre");
	ResourceManager::Instance()->loadMaterial("crome.material", "cromo");
	ResourceManager::Instance()->loadMaterial("ruby.material", "ruby");
	ResourceManager::Instance()->loadMaterial("cristal.material", "cristal");

	// Prueba excepciones
	ResourceManager::Instance()->loadTexture("ladrillo.bmp", "ladrillo");
	ResourceManager::Instance()->loadMaterial("plata.material", "plata");

	/* Shaders que vamos a usar */
	ResourceManager::Instance()->loadShader("default.vert", "cruces.geom", "default.frag", "cruces");
	ResourceManager::Instance()->loadShader("skybox.vert", "", "skybox.frag", "skybox");
	ResourceManager::Instance()->loadShader("maximize.vert", "", "fog.frag", "bigFog");
	ResourceManager::Instance()->loadShader("normals.vert", "normals.geom", "normals.frag", "normals");
	ResourceManager::Instance()->loadShader("default.vert", "", "movimiento.frag", "movimiento");

	/* Efectos de postprocesado (también son shaders) */
	ResourceManager::Instance()->loadShader("postprocess/composite.vert", "", "postprocess/interference.frag", "interference");
	ResourceManager::Instance()->loadShader("postprocess/composite.vert", "", "postprocess/waves.frag", "waves");
	//compositeShader = (Shader*)&ResourceManager::Instance()->getShader("composite");
	// (2, 2) para que ocupe la pantalla entera
	m_effectsMesh = Mesh::generateRectangle(2, 2);
	//ResourceManager::Instance()->loadShader("default.vert", "default.frag", "specularMap.frag", "specMapShader");


	// LUCES
	// Puntual
	Light* spotLight = new Light(SPOT_LIGHT, { 1, 1, 0.8, 1 });
	spotLight->setPosition({ -2.5,0.5,-2.5 });
	spotLight->setActive(true);
	AddLight(spotLight);

	// Puntual (va en círculos)
	Light* circleLight = new Light(POINT_LIGHT, { 0, 0, 1, 1 });
	circleLight->setPosition({ 3,3,-3 });
	AddLight(circleLight);

	// Direccional
	Light* dirLight = new Light(DIRECTIONAL_LIGHT);
	dirLight->setDirection({ -1,0 , 0 });
	//m_dirLight->setAmbient({ 0.2, 0.2, 0.2 ,1 });
	dirLight->setActive(true);
	AddLight(dirLight);

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

	// Peon de ajedrez
	Entity* peon = new Entity();
	peon->setMesh("peon");
	peon->setMaterial("cristal");
	peon->setPosition({ 0,0,-2 });
	AddEntity(peon);


	// Torre de ajedrez
	Entity* torre = new Entity();
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


	/* Canvas */
	m_canvas = new Canvas();
	m_canvas->setSize(800, 600);

	/* - - Canvas - - */
	// Entidad vacía para que cuelguen los botones
	UIElement* botonesMenu = new UIElement();
	botonesMenu->setActive(false);
	m_canvas->addElement(botonesMenu);

	// Botones
	for (int i = 0; i < buttonNames.size(); i++)
	{
		// Su textura, posición, escala y callback
		Button* b = new Button(buttonNames[i], m_canvas);
		if (i < 7)
			b->setPositionUI(buttonSep, 0.89 - i * buttonSep);
		else
			b->setPositionUI(1 - buttonSep, 0.89 - (i - 7) * buttonSep);
		b->setScaleUI(buttonScale, buttonScale);
		b->setCallback(callbacks[i]);
		b->setParent(botonesMenu); // hijos del nodo vacío
	}


	/* - - Skybox - - */
	// El orden tiene que ser este (top y bottom están invertidos por alguna razón)
	std::vector<std::string> lakeFaces = { "right.bmp", "left.bmp", "bottom.bmp", "top.bmp", "front.bmp", "back.bmp" };
	std::vector<std::string> yokohamaFaces = { "city/right.bmp", "city/left.bmp", "city/bottom.bmp",
		"city/top.bmp", "city/front.bmp", "city/back.bmp" };
	m_skybox = new Skybox(yokohamaFaces);
	m_skybox->setShader("skybox");

	// GAMEMANAGER
	GameManager* gm = new GameManager(this, m_camera, botonesMenu, torre);
	gm->setLights(dirLight, circleLight, spotLight);
	AddEntity(gm);
}


void PruebaScene::PruebaMateriales()
{
	// Cubo con la misma textura en todas las caras
	Cubo* c = new Cubo(2, true);
	c->setTexture("caja");
	c->setSpecularMap("caja_spec");
	c->setPosition({ -10,0,0 });
	c->setShader("default");
	AddEntity(c);

	// Cubo de orientación (distintas texturas)
	Cubo* c2 = new Cubo(2, true, false);
	c2->setTexture("orientacion");
	c2->setMaterial("orientacion");
	c2->setPosition({ -5,0,0 });
	AddEntity(c2);

	// Cubo default
	//Cubo* cuboDef = new Cubo(2, false);
	//cuboDef->setMaterial("default");
	//cuboDef->setPosition({ 0,0,0 });
	//AddEntity(cuboDef);

	// Cubo de rubi
	Cubo* cuboRubi = new Cubo(2, false);
	cuboRubi->setMaterial("cromo");
	cuboRubi->setPosition({ 5,0,0 });
	AddEntity(cuboRubi);

	// Cubo de cobre
	Cubo* cuboCobre = new Cubo(2, true);
	//cuboCobre->setTexture("cobre");
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
	tierra->setTexture("earth");
	tierra->setPosition({ 6,15,0 });
	//tierra->rotate(-PI / 2, { 1, 0, 0 }, LOCAL); // 90º horario en eje X local
	tierra->rotate(-PI / 8, { 0, 0, 1 }, GLOBAL);
	tierra->setShader("cruces");
	AddEntity(tierra);

	// Venus
	//Esfera* venus = new Esfera(4, 20, true);
	//venus->setTexture(*venusTex);
	//venus->setPosition({ 15,10,0 });
	//m_entities.push_back(venus);

	// Rejilla (suelo)
	Grid* grid = new Grid(80, 160, 0.25, 0.25);
	grid->setTexture("agua");
	grid->setMaterial("cromo");
	grid->setPosition({ 0,-1,0 });
	//grid->setShader("movimiento");
	AddEntity(grid);

	// "Cascada"
	Grid* vGrid = new Grid(80, 80, 0.25, 0.25);
	vGrid->setTexture("agua");
	vGrid->setPosition({ 20,-11,0 });
	vGrid->rotate(-PI / 2, { 0, 0, 1 }, GLOBAL);
	vGrid->setShader("movimiento");
	AddEntity(vGrid);


	// Terreno
	Terrain* terrain = new Terrain("../bin/assets/terrain.raw", 0.5);
	terrain->setTexture("terreno");
	terrain->setPosition({ 0,-10,0 });
	AddEntity(terrain);
}

void PruebaScene::ViewportTest()
{
	glUseProgram(0);

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


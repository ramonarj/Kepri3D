#include "PruebaScene.h"

#include "../Kepri3D.h"

#include "GameManager.h"

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
	// Hacer que el cursor sea invisible y moverlo al centro de la ventana
	InputManager::Instance()->setCursor(GLUT_CURSOR_NONE);
	InputManager::Instance()->setMousePos(m_camera->getVP()->getW() / 2, m_camera->getVP()->getH() / 2);
	//glutWarpPointer(m_camera->getVP()->getW() / 2, m_camera->getVP()->getH() / 2);

	// CARGA DE RECURSOS PARA ESTA ESCENA
	loadResources();

	// LUCES
	// Direccional
	Light* dirLight = new Light(DIRECTIONAL_LIGHT);
	dirLight->setDirection({ -1,0 , 0 });
	dirLight->setDiffuse({ 1, 1, 1, 1.0 });
	dirLight->setActive(true);
	AddLight(dirLight);

	// Puntual (va en círculos)
	Light* circleLight = new Light(POINT_LIGHT, { 1, 1, 0, 1 });
	circleLight->setPosition({ 3,3,-3 });
	circleLight->setSpecular({ 0, 0, 1, 1.0 });
	AddLight(circleLight);

	// Foco (linterna)
	Light* spotLight = new Light(SPOT_LIGHT, { 0.6, 0.6, 0.6, 1 });
	spotLight->setPosition({ -2.5,0.5,-2.5 });
	//spotLight->setSpecular({ 1, 0, 0, 1.0 });
	spotLight->setActive(true);
	AddLight(spotLight);


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
	toro->setPosition({ 15,-0.4,6 });
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
	hierba->setPosition({ 10, 0.5, 5 });
	hierba->rotate(-PI / 2, { 0, 1, 0 }, GLOBAL);
	hierba->setTexture("hierba");
	//hierba->setShader("lights");
	AddEntity(hierba);

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


	/* - - Skybox - - */
	Skybox* sky = new Skybox("lakeSkybox");
	SetSkybox(sky);

	/* - - Sistema de partículas - - */
	// Con 10.000, empieza a ir demasiado lento
	particleSys = new ParticleSystem(0.4, 1000, PARTICLE_2D);
	particleSys->setTexture("emoji");
	particleSys->setParticleSpeed(3.0);
	particleSys->setLifetime(2);
	particleSys->setActive(false);
	AddEntity(particleSys);

	/* - - GameManager - - */
	GameManager* gm = new GameManager(this, m_camera, botonesMenu, particleSys);
	gm->setLights(dirLight, circleLight, spotLight);
	AddEntity(gm);
}

void PruebaScene::loadResources()
{
	/* Mallas */
	ResourceManager::Instance()->loadMesh("Torre.obj", "torre");
	ResourceManager::Instance()->loadMesh("Peon.obj", "peon");

	/* Texturas */
	ResourceManager::Instance()->loadTexture("earth24.bmp", "earth");
	ResourceManager::Instance()->loadTexture("venus.bmp", "venus");
	ResourceManager::Instance()->loadTexture("orientacion.bmp", "orientacion");
	ResourceManager::Instance()->loadTexture("Zelda.bmp", "zelda");
	ResourceManager::Instance()->loadTexture("terrenoTex.bmp", "terreno");
	ResourceManager::Instance()->loadTexture("iceland.bmp", "iceland", {4, 65, 137});
	ResourceManager::Instance()->loadTexture("caja.bmp", "caja");
	ResourceManager::Instance()->loadTexture("caja_specular.bmp", "caja_spec");
	ResourceManager::Instance()->loadTexture("cobre.bmp", "cobre");
	ResourceManager::Instance()->loadTexture("agua.bmp", "agua", 200);
	ResourceManager::Instance()->loadTexture("emoji.bmp", "emoji", { 0, 0, 0 });
	ResourceManager::Instance()->loadTexture("lego.bmp", "lego");
	ResourceManager::Instance()->loadTexture("grass.bmp", "hierba", { 0,0,0 });

	// Botones
	for (int i = 0; i < buttonNames.size(); i++)
	{
		ResourceManager::Instance()->loadTexture("UI/" + buttonNames[i] + ".bmp", buttonNames[i]);
	}

	// Skyboxes - el orden tiene que ser este (top y bottom están invertidos por alguna razón)
	ResourceManager::Instance()->loadCubemapTexture({ "skyboxes/right.bmp", "skyboxes/left.bmp", "skyboxes/bottom.bmp",
		"skyboxes/top.bmp", "skyboxes/front.bmp", "skyboxes/back.bmp" }, "lakeSkybox");
	ResourceManager::Instance()->loadCubemapTexture({ "skyboxes/city/right.bmp", "skyboxes/city/left.bmp", "skyboxes/city/bottom.bmp",
		"skyboxes/city/top.bmp", "skyboxes/city/front.bmp", "skyboxes/city/back.bmp" }, "citySkybox");

	/* Materiales */
	ResourceManager::Instance()->loadMaterial("copper.material", "cobre");
	ResourceManager::Instance()->loadMaterial("crome.material", "cromo");
	ResourceManager::Instance()->loadMaterial("ruby.material", "ruby");
	ResourceManager::Instance()->loadMaterial("cristal.material", "cristal");

	// Prueba excepciones
	ResourceManager::Instance()->loadTexture("ladrillo.bmp", "ladrillo");
	ResourceManager::Instance()->loadMaterial("plata.material", "plata");

	/* Shaders */
	ResourceManager::Instance()->loadShader("lights.vert", "", "lights.frag", "lights");
	ResourceManager::Instance()->loadShader("default.vert", "cruces.geom", "default.frag", "cruces");
	ResourceManager::Instance()->loadShader("maximize.vert", "", "fog.frag", "bigFog");
	ResourceManager::Instance()->loadShader("default.vert", "", "movimiento.frag", "movimiento");
	ResourceManager::Instance()->loadShader("default.vert", "", "multitexture.frag", "multitexture");
	ResourceManager::Instance()->loadShader("lights.vert", "", "specularMap.frag", "specMap");

	/* Efectos de postprocesado ('composites') */
	ResourceManager::Instance()->loadComposite("interference.frag", "interference");
	ResourceManager::Instance()->loadComposite("waves.frag", "waves");
}


void PruebaScene::PruebaMateriales()
{
	// Cubo con la misma textura en todas las caras
	Cubo* c = new Cubo(2, true);
	c->setTexture("caja");
	c->setPosition({ -10,0,0 });
	c->setShader("lights");
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

	// Cubo de cromo
	Cubo* cuboCromo = new Cubo(2, false);
	cuboCromo->setMaterial("cromo");
	cuboCromo->setPosition({ 5,0,0 });
	AddEntity(cuboCromo);

	// Cubo de cobre
	Cubo* cuboCobre = new Cubo(2, true);
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
	tierra->setTexture("earth");
	tierra->setPosition({ 6,15,0 });
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
	//grid->setShader("lights");
	AddEntity(grid);

	// "Cascada"
	Grid* vGrid = new Grid(80, 80, 0.25, 0.25);
	vGrid->setTexture("agua");
	vGrid->setPosition({ 20,-11,0 });
	vGrid->rotate(-PI / 2, { 0, 0, 1 }, GLOBAL);
	vGrid->setShader("movimiento");
	AddEntity(vGrid);

	// Terreno antiguo
	Terrain* oldTer = new Terrain();
	oldTer->loadRAW("../bin/assets/terrain.raw", 0.05);
	oldTer->setTexture("terreno");
	oldTer->setPosition({ -30,0, 0 });
	AddEntity(oldTer);

	// Terreno Islandia
	Terrain* terrain = new Terrain();
	terrain->loadHeightMap("../bin/assets/icelandHeight.bmp", 1);
	terrain->setTexture("iceland");
	terrain->setPosition({ 0,-10,0 });
	AddEntity(terrain);

	// Minimap
	Terrain* miniTer = new Terrain();
	miniTer->loadHeightMap("../bin/assets/icelandHeight.bmp", 0.01);
	miniTer->setTexture("iceland");
	miniTer->scale({ 1,1.5,1 });
	miniTer->setPosition({ -6,-0.99,6 });
	miniTer->rotate(PI / 2, { 0,1,0 }, GLOBAL);
	AddEntity(miniTer);
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
	switchBoolParam(GL_CULL_FACE);
	centerMouse();
}

void PruebaScene::blendingButtonPressed()
{
	// Activar / desactivar las transparencias en texturas y materiales
	switchBoolParam(GL_BLEND);
	centerMouse();
}

void PruebaScene::lightingButtonPressed()
{
	// Activar / desactivar la iluminación
	switchBoolParam(GL_LIGHTING);
	centerMouse();
}

void PruebaScene::texturesButtonPressed()
{
	// Activar / desactivar el uso de texturas
	switchBoolParam(GL_TEXTURE_2D);
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
	switchBoolParam(GL_ALPHA_TEST);
	centerMouse();
}

void PruebaScene::multisamplingButtonPressed()
{
	// Activar / desactivar el multisampling
	switchBoolParam(GL_MULTISAMPLE);
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
	// Activar / desactivar la visualización de vectores normales a cada vértice
	if (compositeShader == nullptr)
		compositeShader = (Shader*)&ResourceManager::Instance()->getComposite("waves");
	else
		compositeShader = nullptr;

	centerMouse();
}


void PruebaScene::scissorButtonPressed()
{
	// Bandas cinemáticas negras. Como tenemos 2 buffers, hay que ponerlos los 2 a negro antes de
	// activar el scissor test (para que los bordes no se queden con imágenes residuales y parpadeen)
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	// Segundo buffer
	if (glutGet(GLUT_WINDOW_DOUBLEBUFFER) == 1)
	{
		glutSwapBuffers();
		glClear(GL_COLOR_BUFFER_BIT);
	}

	// Establecer la zona visible (en píxeles) del scissor box
	float proporcionBarra = glutGet(GLUT_WINDOW_HEIGHT) / 6.0f;
	glScissor(0, proporcionBarra, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT) - proporcionBarra * 2);

	// Activar/desactivar el scissor test
	switchBoolParam(GL_SCISSOR_TEST);

	// Volver a dejar el color de fondo a blanco
	glClearColor(1, 1, 1, 0);

	centerMouse();
}

void PruebaScene::skyboxButtonPressed()
{
	skyboxActive = !skyboxActive;
	centerMouse();
}

void PruebaScene::gammaButtonPressed()
{
	switchBoolParam(GL_FRAMEBUFFER_SRGB);
	centerMouse();
}

void PruebaScene::stencilButtonPressed()
{
	switchBoolParam(GL_STENCIL_TEST);
	centerMouse();
}

void PruebaScene::logicOpButtonPressed()
{
	// Activar las operaciones lógicas implica desactivar totalmente el 'Blending'
	switchBoolParam(GL_COLOR_LOGIC_OP);

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

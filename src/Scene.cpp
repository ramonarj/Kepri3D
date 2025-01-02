#include "Scene.h"

#include <glm.hpp>

#include "Texture.h"
#include "Entity.h"
#include "Camera.h"
#include "Light.h"
#include "Shader.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "GameManager.h"
#include "UI/Button.h"
#include "UI/Canvas.h"


//#include <glew.h>
#include <freeglut.h>

bool Scene::shadersActive = false;
bool Scene::mipmapsActive = false;
Shader* Scene::normalsShader = nullptr;


void Scene::AddEntity(Entity* e, bool isTranslucid)
{
	//if (!isTranslucid)
	//	m_opaqueEntities.push_back(e);
	//else
	//	m_translucentEntities.push_back(e);
	m_entities.push_back(e);
}

void Scene::init()
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

	ResourceManager::Instance()->loadTexture("botonCulling.bmp", "botonCulling");
	ResourceManager::Instance()->loadTexture("botonBlending.bmp", "botonBlending");
	ResourceManager::Instance()->loadTexture("botonLighting.bmp", "botonLighting");
	ResourceManager::Instance()->loadTexture("botonTextures.bmp", "botonTextures");
	ResourceManager::Instance()->loadTexture("botonShading.bmp", "botonShading");
	ResourceManager::Instance()->loadTexture("botonShaders.bmp", "botonShader");
	ResourceManager::Instance()->loadTexture("botonMultisampling.bmp", "botonMultisampling");
	ResourceManager::Instance()->loadTexture("botonMipmaps.bmp", "botonMipmaps");
	ResourceManager::Instance()->loadTexture("botonNormales.bmp", "botonNormales");

	/* Materiales que vamos a usar */
	ResourceManager::Instance()->loadMaterial("copper.material", "cobre");
	ResourceManager::Instance()->loadMaterial("crome.material", "cromo");
	ResourceManager::Instance()->loadMaterial("ruby.material", "ruby");
	
	// Prueba excepciones
	ResourceManager::Instance()->loadTexture("ladrillo.bmp", "ladrillo");
	ResourceManager::Instance()->loadMaterial("plata.material", "plata");

	/* Shaders que vamos a usar */
	ResourceManager::Instance()->loadShader("default.vert", "cruces.geom", "default.frag", "cruces");
	ResourceManager::Instance()->loadShader("maximize.vert", "", "fog.frag", "bigFog");
	ResourceManager::Instance()->loadShader("normals.vert", "normals.geom", "normals.frag", "normals");
	ResourceManager::Instance()->loadShader("default.vert", "", "movimiento.frag", "movimiento");
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
	peon->setMaterial("cromo");
	peon->setPosition({ 0,0,-2 });
	AddEntity(peon);


	// Torre de ajedrez
	Entity* torre = new Entity();
	torre->setMesh("torre");
	//torre->setTexture("cobre");
	torre->setMaterial("ruby");
	torre->setPosition({ 0,0,-2 });
	AddEntity(torre);
	// Para que el blending del rubí funcione bien (de momento), 
	// tiene que estar la última de las entidades


	/* Canvas */
	m_canvas = new Canvas();
	m_canvas->setSize(800, 600);

	// CANVAS
	// Entidad vacía para que cuelguen los botones
	UIElement* botonesMenu = new UIElement();
	botonesMenu->setActive(false);
	m_canvas->addElement(botonesMenu);
	

	// Culling
	Button* cullButton = new Button("botonCulling", m_canvas);
	cullButton->setPositionUI(0.12, 0.9);
	cullButton->setScaleUI(0.3, 0.3);
	//button->setPositionUI(50, 35);
	cullButton->setCallback(cullingButtonPressed);
	cullButton->setParent(botonesMenu);

	// Blending
	Button* blendButton = new Button("botonBlending", m_canvas);
	blendButton->setPositionUI(0.12, 0.75);
	blendButton->setScaleUI(0.3, 0.3);
	blendButton->setCallback(blendingButtonPressed);
	blendButton->setParent(botonesMenu);

	// Lighting
	Button* lightButton = new Button("botonLighting", m_canvas);
	lightButton->setPositionUI(0.12, 0.6);
	lightButton->setScaleUI(0.3, 0.3);
	lightButton->setCallback(lightingButtonPressed);
	lightButton->setParent(botonesMenu);
	//lightButton->setActive(false);

	// Textures
	Button* texturesButton = new Button("botonTextures", m_canvas);
	texturesButton->setPositionUI(0.12, 0.45);
	texturesButton->setScaleUI(0.3, 0.3);
	texturesButton->setCallback(texturesButtonPressed);
	texturesButton->setParent(botonesMenu);

	// Shading
	Button* shadingButton = new Button("botonShading", m_canvas);
	shadingButton->setPositionUI(0.12, 0.3);
	shadingButton->setScaleUI(0.3, 0.3);
	shadingButton->setCallback(shadingButtonPressed);
	shadingButton->setParent(botonesMenu);

	// Shaders
	Button* shaderButton = new Button("botonShader", m_canvas);
	shaderButton->setPositionUI(0.12, 0.15);
	shaderButton->setScaleUI(0.3, 0.3);
	shaderButton->setCallback(shaderButtonPressed);
	shaderButton->setParent(botonesMenu);

	// Multisampling
	Button* multisamplingButton = new Button("botonMultisampling", m_canvas);
	multisamplingButton->setPositionUI(0.88, 0.9);
	multisamplingButton->setScaleUI(0.3, 0.3);
	multisamplingButton->setCallback(multisamplingButtonPressed);
	multisamplingButton->setParent(botonesMenu);

	// Mipmaps
	Button* mipmapsButton = new Button("botonMipmaps", m_canvas);
	mipmapsButton->setPositionUI(0.88, 0.75);
	mipmapsButton->setScaleUI(0.3, 0.3);
	mipmapsButton->setCallback(mipmapButtonPressed);
	mipmapsButton->setParent(botonesMenu);

	// Visualización de normales
	Button* normalsButton = new Button("botonNormales", m_canvas);
	normalsButton->setPositionUI(0.88, 0.6);
	normalsButton->setScaleUI(0.3, 0.3);
	normalsButton->setCallback(normalsButtonPressed);
	normalsButton->setParent(botonesMenu);

	// GAMEMANAGER
	GameManager* gm = new GameManager(this, m_camera, botonesMenu);
	gm->setLights(dirLight, circleLight, spotLight);
	AddEntity(gm);
}

void Scene::render()
{
	// 0) Limpia el color y el depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 1) Cargar las luces; IMPORTANTE hacerlo antes de pintar los objetos a los que puedan iluminar
	for (Light* l : m_lights)
	{
		if (l->isActive())
		{
			l->load(m_camera->getViewMat());
		}
	}


	// 2) Pintar todas las entidades 
	int mvpMatLoc;
	const Shader* activeShader = nullptr;
	glm::dmat4 projViewMat = m_camera->getProjMat() * m_camera->getViewMat();
	// pintamos todas las entidades activas
	for (Entity* e : m_entities)
	{
		if(e->isActive())
		{
			// la entidad no usa shaders; desactivamos
			if (e->getShader() == nullptr)
				glUseProgram(0);
			// la entidad usa shaders
			else
			{
				// hay que cambiar el shader que usó la entidad anterior
				if (e->getShader() != activeShader)
				{
					activeShader = e->getShader();
					mvpMatLoc = glGetUniformLocation(activeShader->getId(), "mvpMat");
					glUseProgram(activeShader->getId());
				}
				// pasar la matriz MVP al vertex shader
				glUniformMatrix4dv(mvpMatLoc, 1, GL_FALSE, glm::value_ptr(projViewMat * e->getModelMat()));
			}
			// en cualquier caso, mandamos la info de los vértices
			e->render(m_camera->getViewMat());
		}
	}

	// 3) Pintar los vectores normales, si están activos
	if(normalsShader != nullptr)
	{
		glUseProgram(normalsShader->getId());
		mvpMatLoc = glGetUniformLocation(normalsShader->getId(), "mvpMat");
		for (Entity* e : m_entities)
		{
			// Pasar la matriz MVP al vertex shader
			glUniformMatrix4dv(mvpMatLoc, 1, GL_FALSE, glm::value_ptr(projViewMat * e->getModelMat()));
			e->render(m_camera->getViewMat());
		}
	}
	
	// Desactivamos los shaders para el canvas
	glUseProgram(0);

	// 4) Pintar el canvas, limpiando antes el Z-buffer para que se pinte encima de tod
	glClear(GL_DEPTH_BUFFER_BIT);
	m_canvas->render(m_camera->getViewMat());

	//ViewportTest();

	// 5) Hacer swap de buffers
	// Hay 2 buffers; uno se está mostrando por ventana, y el otro es el que usamos
	// para dibujar con la GPU. Cuando se ha terminado de dibujar y llega el siguiente 
	// frame, se intercambian las referencias y se repite el proceso
	glutSwapBuffers();
}

void Scene::update(GLuint deltaTime)
{
	// Actualizar el canvas. Importante hacerlo antes que el resto de entidades
	m_canvas->update(deltaTime);

	// Actualizar las entidades
	for (Entity* e : m_entities)
	{
		if(e->isActive())
		{
			e->update(deltaTime);
		}
	}

	// Limpiar el input para el siguiente frame
	InputManager::Instance()->Update();
}

void Scene::ViewportTest()
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

void Scene::PruebaMateriales()
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
	esfera->setPosition({ 0,2,-3 });
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
	vGrid->rotate(-PI / 2, {0, 0, 1}, GLOBAL);
	vGrid->setShader("movimiento");
	AddEntity(vGrid);


	// Terreno
	Terrain* terrain = new Terrain("../bin/assets/terrain.raw", 0.5);
	terrain->setTexture("terreno");
	terrain->setPosition({ 0,-10,0 });
	AddEntity(terrain);
}

void Scene::takePhoto()
{
	// Leemos la información del front buffer (el que está en pantalla), porque 
	// el trasero podría estar dibujándose y tendríamos 'tearing'0
	Texture::loadColorBuffer(m_camera->getVP()->getW(), m_camera->getVP()->getH(), GL_BACK);
	// Sin embargo, con GL_BACK afecta menos al rendimiento (framerate)

	Texture::save("foto.bmp");

	std::cout << "Se ha hecho una foto" << std::endl;
}

void Scene::switchBoolParam(GLenum param)
{
	GLboolean value;
	glGetBooleanv(param, &value);
	if (value)
		glDisable(param);
	else
		glEnable(param);
}

void Scene::cullingButtonPressed()
{
	// Activar / desactivar el culling de polígonos traseros
	switchBoolParam(GL_CULL_FACE);

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::blendingButtonPressed()
{
	// Activar / desactivar las transparencias en texturas y materiales
	switchBoolParam(GL_BLEND);

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::lightingButtonPressed()
{
	// Activar / desactivar la iluminación
	switchBoolParam(GL_LIGHTING);

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::texturesButtonPressed()
{
	// Activar / desactivar el uso de texturas
	switchBoolParam(GL_TEXTURE_2D);

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::shadingButtonPressed()
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

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::shaderButtonPressed()
{
	shadersActive = !shadersActive;

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::multisamplingButtonPressed()
{
	// Activar / desactivar el multisampling
	switchBoolParam(GL_MULTISAMPLE);

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::mipmapButtonPressed()
{
	// Activar / desactivar el uso de mipmaps
	mipmapsActive = !mipmapsActive;
	ResourceManager::Instance()->enableMipmaps(mipmapsActive);


	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::normalsButtonPressed()
{
	// Activar / desactivar la visualización de vectores normales a cada vértice
	if (normalsShader == nullptr)
		normalsShader = (Shader*)&ResourceManager::Instance()->getShader("normals");
	else
		normalsShader = nullptr;

	InputManager::Instance()->setMousePos(400, 300);
}

Scene::~Scene()
{
	// Borrar el canvas
	delete m_canvas;

	// Borrar las entidades
	CleanVector(m_entities);

	// Borrar las luces
	CleanVector(m_lights);

	// Borrar managers
	InputManager::Instance()->Clean();

	// Borrar las mallas, texturas, materiales y shaders cargados a la escena
	ResourceManager::Instance()->Clean();
}
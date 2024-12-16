#include "Scene.h"
#include <freeglut.h>
#include <glm.hpp>

#include "Entity.h"
#include "Camera.h"
#include "Light.h"
#include "UI/Button.h"

using namespace glm;

GLuint totalTime = 0;

void Scene::initGLSubsystems()
{
	// Color de fondo (el predeterminado es negro)
	glClearColor(1.0, 1.0, 1.0, 0);  // (alpha=1 -> opaque)

	/* OpenGL basic settings */
	// Activa el Z-buffer. Si no lo activáramos, se pintaría todo con el algoritmo
	// del pintor (lo más reciente tapa lo antiguo)
	glEnable(GL_DEPTH_TEST);

	// Activa el uso de texturas 2D
	glEnable(GL_TEXTURE_2D);

	// Activa las transparencias, indicando qué canal usar para ello (SRC_ALPHA).
	// Si no llamáramos a glBlendFunc, se usarían los parámetros por defecto (0, 1) y no
	// habría transparencias
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //src, dest

	// Activa el descarte de fragmentos cuyo alfa no cumpla una cierta condición dada
	// NOTA: en el pipeline, va primero el alpha test y después el blend
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.2);

	/* Antialiasing; tanto para líneas, como para polígonos */
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	
	/* Culling; descarta el dibujado de los polígonos traseros. 
	Lo suyo sería tenerlo siempre activo, y las entidades que necesiten renderizar las 2 caras,
	lo desactivan y vuelven a activar en su 'render' */
	//glEnable(GL_CULL_FACE); 

	/* Iluminación, activa el uso de las luces y sombreado */
	glEnable(GL_LIGHTING);
	// Tipo de modelo de sombreado -> GL_FLAT (flat), GL_SMOOTH (gouraud)
	glShadeModel(GL_SMOOTH);
	// Para no iluminar las caras traseras de las mallas. Por defecto está a false
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	// Punto de vista para la reflexión especular de los materiales
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	// Normalizar los vectores normales
	glEnable(GL_NORMALIZE);
}

void Scene::init()
{
	// Activa el depth test, uso de texturas, iluminación, etc.
	initGLSubsystems();
	
	/* Luces */
	// Puntual
	Light* m_pointLight = new Light({ 1, 1, 0, 1 });
	m_pointLight->setPosition({ -2.5,0.5,-2.5 });
	m_pointLight->setActive(true);
	m_lights.push_back(m_pointLight);

	Light* m_pointLight2 = new Light({ 0, 0, 1, 1 });
	m_pointLight2->setPosition({ 3,3,-3 });
	m_lights.push_back(m_pointLight2);

	// Direccional
	Light* m_dirLight = new Light();
	m_dirLight->setDirection({ -1, -1, -1 });
	m_dirLight->setAmbient({ 0.2, 0.2, 0.2 ,1 });
	m_dirLight->setActive(true);
	m_lights.push_back(m_dirLight);

	/* Texturas que vamos a usar */
	Texture* earthTex = new Texture();
	earthTex->load("earth24.bmp");
	m_textures.push_back(earthTex);

	Texture* venusTex = new Texture();
	venusTex->load("venus.bmp");
	m_textures.push_back(venusTex);

	Texture* orientacionTex = new Texture();
	orientacionTex->load("orientacion.bmp");
	m_textures.push_back(orientacionTex);

	Texture* zeldaTex = new Texture();
	zeldaTex->load("Zelda.bmp");
	m_textures.push_back(zeldaTex);

	Texture* terrenoTex = new Texture();
	terrenoTex->load("terrenoTex.bmp");
	m_textures.push_back(terrenoTex);

	Texture* buttonTex = new Texture();
	buttonTex->load("ray.bmp");
	m_textures.push_back(buttonTex);

	/* Materiales */
	Material* defaultMat = new Material();
	m_materials.push_back(defaultMat);
	Material* cromeMat = new Material({ 0.25, 0.25, 0.25 }, {0.4, 0.4, 0.4}, {0.77, 0.77, 0.77}, 77.0);
	m_materials.push_back(cromeMat);
	Material* cobreMat = new Material({ 0.19, 0.07, 0.02 }, { 0.7, 0.27, 0.08 }, { 0.26, 0.14, 0.086 }, 13.0);
	m_materials.push_back(cobreMat);

	/* Entidades */
	// Ejes RGB
	//m_entities.push_back(new EjesRGB(0.5));

	// Polígono sin rellenar
	//m_entities.push_back(new Poligono(6, 0.5, false));
	
	// Polígono relleno
	Poligono* pol = new Poligono(4, 1, true);
	pol->setTexture(*zeldaTex);
	//m_entities.push_back(pol);

	// Cubo con la misma textura en todas las caras
	Cubo* c = new Cubo(2, true);
	c->setTexture(*zeldaTex);
	c->setPosition({ -10,0,0 });
	m_entities.push_back(c);

	// Cubo con distintas texturas
	Cubo* c2 = new Cubo(2, true, false);
	c2->setTexture(*orientacionTex);
	c2->setPosition({ -5,0,0 });
	m_entities.push_back(c2);

	// Cubo default
	Cubo* cuboDef = new Cubo(2, false);
	cuboDef->setMaterial(*defaultMat);
	cuboDef->setPosition({ 0,0,0 });
	m_entities.push_back(cuboDef);

	// Cubo de cromo
	Cubo* cuboCromo = new Cubo(2, false);
	cuboCromo->setMaterial(*cromeMat);
	cuboCromo->setPosition({ 5,0,0 });
	m_entities.push_back(cuboCromo);

	// Cubo de cobre
	Cubo* cuboCobre = new Cubo(2, false);
	cuboCobre->setMaterial(*cobreMat);
	cuboCobre->setPosition({ 10,0,0 });
	m_entities.push_back(cuboCobre);

	// Esfera
	Esfera* esfera = new Esfera(1, 8);
	esfera->setPosition({ 0,0,-2 });
	//m_entities.push_back(esfera);

	// Tierra
	Esfera* tierra = new Esfera(3, 20, true);
	tierra->setTexture(*earthTex);
	tierra->setPosition({ 4,10,0 });
	//tierra->rotate(-PI / 2, { 1, 0, 0 }, LOCAL); // 90º horario en eje X local
	tierra->rotate(-PI / 8, { 0, 0, 1 }, GLOBAL);
	m_entities.push_back(tierra);

	// Venus
	//Esfera* venus = new Esfera(4, 20, true);
	//venus->setTexture(*venusTex);
	//venus->setPosition({ 15,10,0 });
	//m_entities.push_back(venus);

	// Rejilla (suelo)
	Grid* grid = new Grid(80, 160, 0.25, 0.25);
	grid->setMaterial(*cromeMat);
	grid->setPosition({ 0,-1,0 });
	m_entities.push_back(grid);


	// Terreno
	Terrain* terrain = new Terrain("../bin/assets/terrain.raw", 0.5);
	terrain->setTexture(*terrenoTex);
	terrain->setPosition({ 0,-10,0 });
	m_entities.push_back(terrain);


	// Botón: prueba
	Button* button = new Button(buttonTex);
	button->scale({ 0.5, 0.3, 1 });
	button->setPositionUI(0.15, 0.8);
	m_entities.push_back(button);
}

void Scene::render()
{
	// Limpia el color y el depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Cargar las luces; IMPORTANTE hacerlo antes de pintar los objetos a los que puedan iluminar
	for (Light* l : m_lights)
		l->load(m_camera->getViewMat());

	//Pintar todas las entidades
	for (Entity* e : m_entities)
		e->render(m_camera->getViewMat());

	//ViewportTest();

	// Hay 2 buffers; uno se está mostrando por ventana, y el otro es el que usamos
	// para dibujar con la GPU. Cuando se ha terminado de dibujar y llega el siguiente 
	// frame, se intercambian las referencias y se repite el proceso
	glutSwapBuffers();
}

void Scene::update(GLuint deltaTime)
{
	for (Entity* e : m_entities)
	{
		e->update(deltaTime);
	}

	// Animación para la luz
	m_lights[1]->setPosition({15 * cos(totalTime * 0.002), 1, 5 * sin(totalTime * 0.002)});
	totalTime += deltaTime;
}

void Scene::ViewportTest()
{

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

Scene::~Scene()
{
	// Borrar las entidades
	for(Entity * it : m_entities)
	{
		delete it;
	}
	m_entities.clear();

	// Borrar los materiales
	for (Material* it : m_materials)
	{
		delete it;
	}
	m_materials.clear();

	// Borrar las texturas
	for (Texture* it : m_textures)
	{
		delete it;
	}
	m_textures.clear();

	// Desactivar los parámetros de OpenGL
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
}
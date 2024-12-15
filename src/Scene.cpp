#include "Scene.h"
#include <freeglut.h>
#include <glm.hpp>

#include "Entity.h"
#include "Camera.h"

using namespace glm;

void Scene::init()
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
	glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	//glEnable(GL_CULL_FACE); //cuidado con esto

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

	/* Entidades */
	// Ejes RGB
	m_entities.push_back(new EjesRGB(0.5));

	// Polígono sin rellenar
	//m_entities.push_back(new Poligono(6, 0.5, false));
	
	// Polígono relleno
	Poligono* pol = new Poligono(4, 1, true);
	pol->setTexture(*zeldaTex);
	m_entities.push_back(pol);

	// Cubo con la misma textura en todas las caras
	Cubo* c = new Cubo(1, true);
	c->setTexture(*zeldaTex);
	c->setPosition({ -1.5,0,0 });
	m_entities.push_back(c);

	// Cubo con distintas texturas
	Cubo* c2 = new Cubo(1, true, false);
	c2->setTexture(*orientacionTex);
	c2->setPosition({ 1.5,0,0 });
	m_entities.push_back(c2);

	// Suelo
	//Poligono* suelo = new Poligono(4, 8, true);
	//suelo->setTexture(*zeldaTex);
	//suelo->setPosition({ 0,-0.5,0 });
	//suelo->rotate(-3.1416 / 2, dvec3(1,0,0));
	//m_entities.push_back(suelo);

	// Esfera
	Esfera* esfera = new Esfera(1, 8);
	esfera->setPosition({ 0,0,-2 });
	m_entities.push_back(esfera);

	// Tierra
	Esfera* tierra = new Esfera(3, 20, true);
	tierra->setTexture(*earthTex);
	tierra->setPosition({ 4,10,0 });
	//tierra->rotate(PI / 2, { 0, 1, 0 }, LOCAL);
	//tierra->rotate(-PI / 2, { 1, 0, 0 }, LOCAL); // 90º horario en eje X local
	tierra->rotate(-PI / 8, { 0, 0, 1 }, GLOBAL);
	//tierra->translate({ 0, 5, 0 }, LOCAL);
	//tierra->setPosition({ -2,0,0 });
	//tierra->scale({ 1, 0.5, 1 });

	m_entities.push_back(tierra);

	// Venus
	//Esfera* venus = new Esfera(4, 20, true);
	//venus->setTexture(*venusTex);
	//venus->setPosition({ 15,10,0 });
	//m_entities.push_back(venus);

	// Rejilla
	Grid* grid = new Grid(20, 3, 0.2, 0.8);
	grid->setPosition({ 0,-1,0 });
	m_entities.push_back(grid);

	// Terreno
	Terrain* terrain = new Terrain("../bin/assets/terrain.raw", 0.5);
	terrain->setTexture(*terrenoTex);
	terrain->setPosition({ 0,-5,0 });
	m_entities.push_back(terrain);
}

void Scene::render()
{
	//Pintar todas las entidades
	for (Entity* e : m_entities)
		e->render(m_camera->getViewMat());

	//ViewportTest();
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

void Scene::update(GLuint timeElapsed)
{
	for(Entity* e : m_entities)
	{
		e->update(timeElapsed);
	}
}

Scene::~Scene()
{
	// Borrar las entidades
	for(Entity * it : m_entities)
	{
		delete it;
	}
	m_entities.clear();

	// Borrar las texturas
	for (Texture* it : m_textures)
	{
		delete it;
	}
	m_textures.clear();

	// Desactivar los parámetros de OpenGL
	glDisable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
}
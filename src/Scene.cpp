#include "Scene.h"
#include <freeglut.h>
#include <glm.hpp>

using namespace glm;

void Scene::init()
{
	// Color de fondo (el predeterminado es negro)
	glClearColor(1.0, 1.0, 1.0, 0);  // (alpha=1 -> opaque)

	/* OpenGL basic settings */
	// Activa el Z-buffer. Si no lo activ�ramos, se pintar�a todo con el algoritmo
	// del pintor (lo m�s reciente tapa lo antiguo)
	glEnable(GL_DEPTH_TEST);

	// Activa el uso de texturas 2D
	glEnable(GL_TEXTURE_2D);

	// Activa las transparencias, indicando qu� canal usar para ello (SRC_ALPHA).
	// Si no llam�ramos a glBlendFunc, se usar�an los par�metros por defecto (0, 1) y no
	// habr�a transparencias
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //src, dest

	// Activa el descarte de fragmentos cuyo alfa no cumpla una cierta condici�n dada
	// NOTA: en el pipeline, va primero el alpha test y despu�s el blend
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.2);

	/* Antialiasing; tanto para l�neas, como para pol�gonos */
	glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	//glEnable(GL_CULL_FACE); //cuidado con esto

	/* Crear y meter todas las entidades */
	// Ejes RGB
	m_entities.push_back(new EjesRGB(0.5));
	//m_entities.push_back(new Poligono(6, 0.5, false));
	// Pol�gono relleno
	m_entities.push_back(new Poligono(4, 1, true, "Zelda.bmp"));

	// Cubo con la misma textura en todas las caras
	Cubo* c = new Cubo(1, "Zelda.bmp");
	c->setPosition({ -1.5,0,0 });
	m_entities.push_back(c);

	// Cubo con distintas texturas
	Cubo* c2 = new Cubo(1, "orientacion.bmp", false);
	c2->setPosition({ 1.5,0,0 });
	m_entities.push_back(c2);

	// Suelo
	Poligono* suelo = new Poligono(4, 8, true, "Zelda.bmp");
	suelo->setPosition({ 0,-0.5,0 });
	suelo->rotate(-3.1416 / 2, dvec3(1,0,0));
	//m_entities.push_back(suelo);

	// Esfera
	Esfera* esfera = new Esfera(20);
	//esfera->setPosition({ 0,-0.5,0 });
	//esfera->rotate(-3.1416 / 2, dvec3(1, 0, 0));
	m_entities.push_back(esfera);

	// Rejilla
	Grid* grid = new Grid(20, 3, 0.2, 0.8);
	grid->setPosition({ 0,-1,0 });
	//esfera->rotate(-3.1416 / 2, dvec3(1, 0, 0));
	m_entities.push_back(grid);
}

void Scene::render()
{
	//Pintar todas las entidades
	for (Entity* e : m_entities)
		e->render(m_camera->getViewMat());

	//Algunas variables locales para facilitar la puesta en escena
	Viewport * view = m_camera->getVP();
	GLdouble w = view->getW();
	GLdouble h = view->getH();
	// Reducimos el tama�o del puerto a 1/4 de la pantalla y dibujamos en los 3 primeros cuadrantes
	//view->setSize(w / 2, h / 2);

	// Ejes RGB
	//view->setPosition(0, h/2);
	//m_entities[0]->render(m_camera->getViewMat());

	//// Pol�gono sin rellenar
	//view->setPosition(w / 2, h / 2);
	//m_entities[1]->render(m_camera->getViewMat());

	//// Pol�gono relleno
	//view->setPosition(0, 0); // 0,0 es abajo a la izquierda
	//m_entities[2]->render(m_camera->getViewMat());
	//// Podemos dibujar un mismo objeto todas las veces que queramos (ejes extra)
	//m_entities[0]->render(m_camera->getViewMat()); 


	view->setSize(w, h); //Volvemos a dejar el viewPort como estaba

	//view->setPosition(w / 2, 0);
	//objetos[3]->render(camera->getViewMat());
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

	// Desactivar los par�metros de OpenGL
	glDisable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
}
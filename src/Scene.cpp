#include "Scene.h"
#include <freeglut.h>
#include <glm.hpp>

using namespace glm;

void Scene::init()
{
	// Color de fondo (el predeterminado es negro)
	glClearColor(1.0, 1.0, 1.0, 0);  // (alpha=1 -> opaque)

	/* OpenGL basic settings */
	// Activa el Z-buffer. Si no lo activáramos, se pintaría todo con el algoritmo
	// del pintor (lo más reciente tapa lo antiguo)
	glEnable(GL_DEPTH_TEST);
	//
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

	//glEnable(GL_CULL_FACE); //cuidado con esto

	// Crear y meter todas las entidades
	m_entities.push_back(new EjesRGB(0.5));
	m_entities.push_back(new Poligono(4, false));
	m_entities.push_back(new Poligono(50, true));
	m_entities.push_back(new Cubo(0.5));
}

void Scene::render()
{
	//Pintar todas las entidades
	//for (Entity* e : m_entities)
	//	e->render();

	//Algunas variables locales para facilitar la puesta en escena
	Viewport * view = m_camera->getVP();
	GLdouble w = view->getW();
	GLdouble h = view->getH();
	// Reducimos el tamaño del puerto a 1/4 de la pantalla y dibujamos en los 3 primeros cuadrantes
	//view->setSize(w / 2, h / 2);

	// Ejes RGB
	//view->setPosition(0, h/2);
	//m_entities[0]->render(m_camera->getViewMat());

	//// Polígono sin rellenar
	//view->setPosition(w / 2, h / 2);
	//m_entities[1]->render(m_camera->getViewMat());

	//// Polígono relleno
	//view->setPosition(0, 0); // 0,0 es abajo a la izquierda
	//m_entities[2]->render(m_camera->getViewMat());
	//// Podemos dibujar un mismo objeto todas las veces que queramos (ejes extra)
	//m_entities[0]->render(m_camera->getViewMat()); 

	// Cubo sin rellenar
	m_entities[0]->render(m_camera->getViewMat());
	m_entities[3]->render(m_camera->getViewMat());

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

	// Desactivar los parámetros de OpenGL
	glDisable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
}
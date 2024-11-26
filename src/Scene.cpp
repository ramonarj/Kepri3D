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
	//
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

	//glEnable(GL_CULL_FACE); //cuidado con esto

	// Crear y meter todas las entidades
	m_entities.push_back(new EjesRGB(0.5));
	m_entities.push_back(new Poligono(5));
}

void Scene::render()
{
	// Pintar todas las entidades
	for (Entity* e : m_entities)
		e->render();
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
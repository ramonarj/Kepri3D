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
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	//glEnable(GL_CULL_FACE); //cuidado con esto

	// Ejemplo ejes RGB
	float l = 0.5;
	numVertices = 8;

	// COORDENADAS:
	// X positiva hacia la derecha
	// Y positiva hacia arriba
	// Z positiva hacia el fondo

	/* Array de vértices */
	vertices = new dvec3[numVertices];
	vertices[0] = dvec3(0, 0, 0);
	vertices[1] = dvec3(l, 0, 0);
	vertices[2] = dvec3(0, 0, 0);
	vertices[3] = dvec3(0, l, 0);
	vertices[4] = dvec3(0, 0, 0);
	vertices[5] = dvec3(0, 0, l);

	// Para probar el DEPTH_TEST
	vertices[6] = dvec3(-2 * l,0, l); // muy lejos
	vertices[7] = dvec3(2* l, 0, l);

	/* Colores para cada vértice */
	colores = new dvec3[numVertices];
	colores[0] = dvec3(1, 0, 0); //rojo
	colores[1] = dvec3(1, 0, 0);
	colores[2] = dvec3(0, 1, 0); //verde
	colores[3] = dvec3(0, 1, 0);
	colores[4] = dvec3(0, 0, 1); //azul
	colores[5] = dvec3(0, 0, 1);

	colores[6] = dvec3(0,0,0); //negro
	colores[7] = dvec3(0,0,0);
}

void Scene::render()
{
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_DOUBLE, 0, colores);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_DOUBLE, 0, vertices);
	// Dibuja los vértices
	glDrawArrays(GL_LINES, 0, numVertices);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

Scene::~Scene()
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
}
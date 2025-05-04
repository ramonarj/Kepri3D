#include "Entity_test.h"

#include "Entity.h"
#include "Graficos/Renderer.h"
#include "Fisica/Rigid.h"

#include <iostream>
#include <assert.h>

void test_addComponent()
{
	Entity e;
	// Componente nullptr
	e.addComponent(nullptr);
	assert(e.getComponents().size() == 0);
	assert(e.getComponent<Renderer>() == nullptr);

	// Componente Renderer
	Renderer* r = new Renderer();
	e.addComponent(r);
	assert(e.getComponents().size() == 1);
	assert(e.getComponent<Renderer>() == r);
	assert(e.getComponent<Rigid>() == nullptr);
}

void test_right()
{
	Entity e;
	assert(glm::length(e.right()) == 1);

	// Traslaciones varias
	e.translate({ -10, -15.5, 10 });
	assert(sameVector(e.right(), { 1, 0, 0 }));

	// Rotaciones varias
	assert(sameVector(e.right(), { 1, 0, 0 }));
	e.rotate(PI, { 1, 0, 0 }, GLOBAL);
	assert(sameVector(e.right(), { 1, 0, 0 }));
	e.rotate(PI / 2, { 0, 1, 0 }, GLOBAL);
	assert(sameVector(e.right(), {0, 0, -1}));
}

void test_up()
{
	Entity e;
	assert(glm::length(e.up()) == 1);

	assert(sameVector(e.up(), { 0, 1, 0 }));
	e.rotate(-PI/2, { 0, 1, 0 }, LOCAL);
	assert(sameVector(e.up(), { 0, 1, 0 }));
}

void test_forward()
{
	Entity e;
	assert(glm::length(e.forward()) == 1);

	// Rotaciones varias
	assert(sameVector(e.forward(), {0, 0, 1}));
}

void test_setParent()
{
	Entity e, e2;
	Entity* parent = new Entity();

	// Sin padre
	assert(e.getParent() == nullptr);
	e = Entity("Manolo");
	assert(e.getParent() == nullptr);
	e = Entity({}, "Manolo");
	assert(e.getParent() == nullptr);
	e = Entity(e2);
	assert(e.getParent() == nullptr);

	// Con padre
	e.setParent(parent);
	assert(e.getParent() == parent);
	assert(parent->getChildren().size() == 1);
	assert(parent->getChildren()[0] == &e);
}
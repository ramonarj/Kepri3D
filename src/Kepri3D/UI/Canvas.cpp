#include "Canvas.h"
#include "UIElement.h"


Canvas::Canvas() : width(0), height(0)
{

}

void Canvas::addElement(UIElement* e) 
{ 
	m_elements.push_back(e); 
	e->setCanvas(this); 
}

void Canvas::render(glm::dmat4 const& viewMat)
{
	for (UIElement* e : m_elements)
	{
		if(e->isActive())
			e->render(viewMat);
	}
}

void Canvas::update(GLuint deltaTime)
{
	for (UIElement* e : m_elements)
	{
		if(e->isActive())
			e->update(deltaTime);
	}
}

Canvas::~Canvas()
{
	// Borrar los elementos UI
	CleanVector(m_elements);
}
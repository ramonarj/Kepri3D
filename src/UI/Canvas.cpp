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
		e->render(viewMat);
}
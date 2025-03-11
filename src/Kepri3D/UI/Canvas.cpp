#include "Canvas.h"

#include "UIElement.h"
#include "Shader.h"
#include "ResourceManager.h"


Canvas::Canvas() : width(0), height(0)
{
	canvasSh = (Shader*)&ResourceManager::Instance()->getShader("UI");
}

void Canvas::addElement(UIElement* e) 
{ 
	m_elements.push_back(e); 
	e->setCanvas(this); 
}

void Canvas::render()
{
	canvasSh->use();
	for (UIElement* e : m_elements)
	{
		if(e->isActive())
			e->render(canvasSh);
	}
	Shader::turnOff();
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
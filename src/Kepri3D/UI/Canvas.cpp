#include "Canvas.h"

#include "UIElement.h"
#include "Shader.h"
#include "ResourceManager.h"
#include "Game.h"


Canvas::Canvas() : width(0), height(0)
{
	canvasSh = ResourceManager::Instance()->getShader("UI");
	// Por defecto, hacemos que el tamaño del canvas coincida con el de la ventana
	glm::ivec2 windowSize = Game::Instance()->getWindowSize();
	setSize(windowSize.x, windowSize.y);
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

void Canvas::update(float deltaTime)
{
	for (UIElement* e : m_elements)
	{
		if(e->isActive())
			e->update(deltaTime);
	}
}

void Canvas::resize(int newWidth, int newHeight)
{
	glm::vec2 changeRate = { (float)newWidth / width, (float)newHeight / height };
	for (UIElement* e : m_elements)
	{
		e->resize(changeRate.x, changeRate.y);
	}
	setSize(newWidth, newHeight);
}

Canvas::~Canvas()
{
	// Borrar los elementos UI
	CleanVector(m_elements);
}
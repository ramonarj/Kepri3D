#include "UIElement.h"

#include "Mesh.h"
#include "Texture.h"
#include "Renderer.h"
#include "Shader.h"

#include <gtc/type_ptr.hpp>

UIElement::UIElement() : x(0), y(0), width(0), height(0), canvas(nullptr)
{
	setPosition({ 0, 0, 0});
}

void UIElement::setPositionUI(float x, float y, ALLIGNMENT_TYPE allignment)
{
	// Cambiamos la última columna, que contiene la posición
	//pasar de (0, 1) a (-1, 1)
	modelMat[3][0] = x * 2 - 1;
	modelMat[3][1] = (y * 2 - 1) * (1 / canvas->aspectRatio());

	this->x = canvas->getWidth() * x;
	this->y = canvas->getHeight() * y;
}

void UIElement::setScaleUI(float x, float y)
{
	Entity::scale({ x, y, 1 });

	width *= x;
	height *= y;
}

void UIElement::render(Shader* sh)
{
	// 1) Renderizar la propia entidad
	if (m_renderer != nullptr)
	{
		// Cargar el material
		m_material.loadToShader(sh);

		// Dibujar la/s malla/s
		sendUniforms(sh);
		sh->setInt("text", false);
		m_renderer->draw();
	}

	// 2) Renderizar sus hijos con el mismo shader dado
	for (Entity* e : m_children)
		if (e->isActive())
			e->render(sh);
}
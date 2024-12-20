#include "UIElement.h"

#include <gtc/type_ptr.hpp>

UIElement::UIElement() : x(0), y(0), width(0), height(0), canvas(nullptr)
{
	// Para estar siempre visible por la cámara (sobra)
	setPosition({ 0, 0, -1 }); // tiene que coincir con -nearPlane
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

void UIElement::render(glm::dmat4 const& viewMat)
{
	m_texture.bind(GL_REPLACE); // a los elementos del canvas NO les afecta la iluminación (no usamos MODULATE)

	// Cargar la matriz de modelado
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(value_ptr(modelMat));

	// Dibujar la/s malla/s
	if (m_mesh != nullptr)
		m_mesh->draw();

	m_texture.unbind();
}

void UIElement::update(GLuint deltaTime)
{

}
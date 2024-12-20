#include "UIElement.h"

#include <gtc/type_ptr.hpp>

UIElement::UIElement() : x(0), y(0)
{

}

void UIElement::setPositionUI(float x, float y, ALLIGNMENT_TYPE allignment)
{
	// Cambiamos la última columna, que contiene la posición
	modelMat[3][0] = x * 2 - 1; //pasar de (0, 1) a (-1, 1)
	modelMat[3][1] = y * 2 - 1;
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
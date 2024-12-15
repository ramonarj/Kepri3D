#include "Button.h"

#include <gtc/type_ptr.hpp>

Button::Button(Texture* texture)
{
	m_mesh = Mesh::generateFilledPolygon(4, 1);
	setTexture(*texture);

	// Para estar siempre visible por la cámara
	setPosition({ 0, 0, -1 });
}

void Button::render(glm::dmat4 const& viewMat)
{
	m_texture.bind();

	// Cargar la matriz de modelado
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(value_ptr(modelMat));

	// Dibujar la/s malla/s
	if (m_mesh != nullptr)
		m_mesh->draw();

	m_texture.unbind();
}

void Button::setPositionUI(float x, float y, ALLIGNMENT_TYPE allignment)
{
	// Cambiamos la última columna, que contiene la posición
	modelMat[3][0] = x * 2 - 1; //pasar de (0, 1) a (-1, 1)
	modelMat[3][1] = y * 2 - 1;
}
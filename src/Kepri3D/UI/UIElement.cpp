#include "UIElement.h"

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
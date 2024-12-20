#ifndef __UI_ELEMENT__
#define __UI_ELEMENT__

#include "../Entity.h"
#include "Canvas.h"
#include <glm.hpp>

class Canvas;

enum ALLIGNMENT_TYPE
{
	LEFT = 0,
	CENTER = 1,
	RIGHT = 2
};

class UIElement : public Entity
{
public:
	UIElement();

	/* Establece la referencia al canvas */
	inline void setCanvas(Canvas* c) { canvas = c; }

	/* Establece la posici�n del bot�n, relativa a la pantalla, y alineado a la izquierda (0 < x, y < 1)
	-> (0, 0) es la esquina inferior izquierda */
	void setPositionUI(float x, float y, ALLIGNMENT_TYPE allignment = LEFT);

	void render(glm::dmat4 const& viewMat);
protected:
	/* Referencia al canvas que pertenece */
	Canvas* canvas;

	/* Posici�n que ocupa en el canvas */
	int x;
	int y;

	/* Dimensiones, en p�xeles, que ocupa respecto al tama�o de referencia del canvas */
	int width;
	int height;
};

#endif
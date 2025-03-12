#ifndef __UI_ELEMENT__
#define __UI_ELEMENT__

#include "Entity.h"
#include "Canvas.h"

enum ALLIGNMENT_TYPE
{
	ALLIGN_LEFT = 0,
	ALLIGN_CENTER = 1,
	ALLIGN_RIGHT = 2
};

class UIElement : public Entity
{
public:
	/* Constructora por defecto */
	UIElement();
	virtual ~UIElement(){}

	/* Establece la referencia al canvas */
	inline void setCanvas(Canvas* c) { canvas = c; }

	/* Establece la posición del botón, relativa a la pantalla, y alineado a la izquierda (0 < x, y < 1)
	-> (0, 0) es la esquina inferior izquierda */
	void setPositionUI(float x, float y, ALLIGNMENT_TYPE allignment = ALLIGN_CENTER);

	void setScaleUI(float x, float y);

protected:
	/* Referencia al canvas que pertenece */
	Canvas* canvas;

	/* Posición que ocupa en el canvas */
	int x;
	int y;

	/* Dimensiones, en píxeles, que ocupa respecto al tamaño de referencia del canvas */
	int width;
	int height;
};

#endif
#ifndef __UI_ELEMENT__
#define __UI_ELEMENT__

#include "../Entity.h"
#include <glm.hpp>

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

	/* Establece la posición del botón, relativa a la pantalla, y alineado a la izquierda (0 < x, y < 1)
	-> (0, 0) es la esquina inferior izquierda */
	void setPositionUI(float x, float y, ALLIGNMENT_TYPE allignment = LEFT);

	void render(glm::dmat4 const& viewMat);
private:
	int x;
	int y;
};

#endif
#ifndef __BUTTON__
#define __BUTTON__

#include "../Mesh.h"
#include "../Entity.h"
#include "../Camera.h"

enum ALLIGNMENT_TYPE
{
	LEFT = 0,
	CENTER = 1,
	RIGHT = 2
};

class Button : public Entity
{
public:
	/* Constructora por defecto */
	Button(std::string textureName);

	/* Establece la posición del botón, relativa a la pantalla, y alineado a la izquierda (0 < x, y < 1)
	-> (0, 0) es la esquina inferior izquierda */
	void setPositionUI(float x, float y, ALLIGNMENT_TYPE allignment = LEFT);

	void render(glm::dmat4 const& viewMat) override;
private:
};

#endif
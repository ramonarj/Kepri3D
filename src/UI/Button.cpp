#include "Button.h"


Button::Button(std::string textureName)
{
	m_mesh = Mesh::generateFilledPolygon(4, 1);
	setTexture(textureName);

	// Para estar siempre visible por la c�mara
	setPosition({ 0, 0, -1 });
}

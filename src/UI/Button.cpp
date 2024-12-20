#include "Button.h"


Button::Button(std::string textureName, Canvas* canvas) : m_callback(nullptr)
{
	// Coger el tama�o en p�xeles de la textura
	setTexture(textureName);
	width = m_texture.getWidth();
	height = m_texture.getHeight();

	// 
	double aspectRatio = (double)canvas->getWidth() / canvas->getHeight();
	m_mesh = Mesh::generateButton((double)width / canvas->getWidth() * 2,
		(double)height / canvas->getHeight() * 2 / aspectRatio);


	// Para estar siempre visible por la c�mara (sobra)
	setPosition({ 0, 0, -1 }); // tiene que coincir con -nearPlane

	// A�adirlo al canvas
	canvas->addElement(this);
}

#include "Button.h"
#include "../Texture.h"

#include "../InputManager.h"


Button::Button(std::string textureName, Canvas* canvas) : m_callback(nullptr)
{
	// Coger el tama�o en p�xeles de la textura
	setTexture(textureName);
	width = m_texture->getWidth();
	height = m_texture->getHeight();

	// 
	double aspectRatio = (double)canvas->getWidth() / canvas->getHeight();
	m_mesh = Mesh::generateButton((double)width / canvas->getWidth() * 2,
		(double)height / canvas->getHeight() * 2 / aspectRatio);

	// A�adirlo al canvas
	canvas->addElement(this);
}

void Button::update(GLuint deltaTime)
{
	// Clic izquierdo del rat�n
	if(InputManager::Instance()->getMouseKeyDown(LEFT))
	{
		glm::ivec2 mousePos = InputManager::Instance()->getMousePos();
		mousePos.y = canvas->getHeight() - mousePos.y;
		//std::cout << "{ " << mousePos.x << ", " << mousePos.y << " }" << std::endl;
		// Se ha pulsado dentro del rect�ngulo del bot�n
		if(mousePos.x > (x - width / 2.0) && mousePos.x < (x + width / 2.0) &&
			mousePos.y > (y - height / 2.0) && mousePos.y < (y + height / 2.0))
		{
			m_callback();
		}
	}
}

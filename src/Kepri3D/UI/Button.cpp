#include "Button.h"
#include "Texture.h"

#include "InputManager.h"


Button::Button(const std::string& textureName, Canvas* canvas) : m_callback(nullptr)
{
	// Coger el tamaño en píxeles de la textura
	setTexture(textureName);
	width = m_textures[0]->getWidth();
	height = m_textures[0]->getHeight();

	// 
	double aspectRatio = (double)canvas->getWidth() / canvas->getHeight();
	m_mesh = Mesh::generateRectangle((double)width / canvas->getWidth() * 2,
		(double)height / canvas->getHeight() * 2 / aspectRatio);

	// Referencia al canvas
	setCanvas(canvas);
}

void Button::update(GLuint deltaTime)
{
	// Clic izquierdo del ratón
	if(InputManager::Instance()->getMouseKeyDown(LEFT))
	{
		glm::ivec2 mousePos = InputManager::Instance()->getMousePos();
		mousePos.y = canvas->getHeight() - mousePos.y;
		//std::cout << "{ " << mousePos.x << ", " << mousePos.y << " }" << std::endl;
		// Se ha pulsado dentro del rectángulo del botón
		if(mousePos.x > (x - width / 2.0) && mousePos.x < (x + width / 2.0) &&
			mousePos.y > (y - height / 2.0) && mousePos.y < (y + height / 2.0))
		{
			m_callback();
		}
	}
}

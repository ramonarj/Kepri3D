#include "Button.h"
#include "Texture.h"

#include "InputManager.h"
#include "ResourceManager.h"
#include "Renderer.h"

Button::Button(const std::string& textureName, Canvas* canvas) : m_callback(nullptr)
{
	// Coger el tamaño en píxeles de la textura
	texture = (Texture*)&ResourceManager::Instance()->getTexture(textureName);
	width = texture->getWidth();
	height = texture->getHeight();

	// 
	double aspectRatio = (double)canvas->getWidth() / canvas->getHeight();
	m_renderer = new Renderer(Mesh::generateRectangle((double)width / canvas->getWidth() * 2,
		(double)height / canvas->getHeight() * 2 / aspectRatio));
	addComponent(m_renderer);

	// Referencia al canvas
	setCanvas(canvas);
}

void Button::update(GLuint deltaTime)
{
	if (m_callback == nullptr)
		return;
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

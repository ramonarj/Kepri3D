#include "Button.h"
#include "Texture.h"

#include "InputManager.h"
#include "ResourceManager.h"
#include "Renderer.h"

Button::Button(unsigned int w, unsigned int h, Canvas* canvas) : m_callback(nullptr)
{
	// Dimenssiones
	width = w;
	height = h;

	// Generar la malla para el renderer
	//double aspectRatio = (double)canvas->getWidth() / canvas->getHeight();
	m_renderer = new Renderer(Mesh::generateRectangle((double)width / canvas->getWidth() * 2,
		(double)height / canvas->getHeight() * 2));
	addComponent(m_renderer);

	// Material UI
	getMaterial()->setInt("text", false);

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

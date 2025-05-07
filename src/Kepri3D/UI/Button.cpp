#include "Button.h"
#include "Texture.h"

#include "InputManager.h"
#include "ResourceManager.h"
#include "Renderer.h"

#include "Mesh.h"
#include "Camera.h"

Button::Button(unsigned int w, unsigned int h, Canvas* canvas) : m_callback(nullptr), m_callback2(nullptr), m_caller(nullptr),
	m_hover(false)
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

void Button::update(float deltaTime)
{
	if (m_callback == nullptr && m_callback2 == nullptr)
		return;

	// Intentamos optimizarlo un poco
	if(m_hoverTex == nullptr)
	{
		// Clic izquierdo del ratón
		if (InputManager::Instance()->getMouseKeyDown(LEFT))
		{
			glm::ivec2 mousePos = InputManager::Instance()->getMousePos();
			mousePos.y = canvas->getHeight() - mousePos.y;

			// Se ha pulsado dentro del rectángulo del botón
			if (insideRect(mousePos.x, mousePos.y))
			{
				callAllCallbacks();
			}
		}
	}

	else
	{
		// Calcular el rectángulo
		glm::ivec2 mousePos = InputManager::Instance()->getMousePos();
		mousePos.y = canvas->getHeight() - mousePos.y;

		//std::cout << mousePos.x << ", " << mousePos.y << std::endl;
		// Entrar en el hover
		if (insideRect(mousePos.x, mousePos.y))
		{
			getMaterial()->setTexture(DIFFUSE_MAP, m_hoverTex);
			m_hover = true;
			if (InputManager::Instance()->getMouseKeyDown(LEFT))
			{
				callAllCallbacks();
			}
		}
		// Salir del hover
		else if(m_hover)
		{
			getMaterial()->setTexture(DIFFUSE_MAP, m_texture);
			m_hover = false;
		}
	}
}

void Button::setTexture(const std::string& textureID)
{
	m_texture = ResourceManager::Instance()->getTexture(textureID);
	getMaterial()->setTexture(DIFFUSE_MAP, m_texture);
}

void Button::setHoverTexture(const std::string& textureID)
{
	m_hoverTex = ResourceManager::Instance()->getTexture(textureID);
}

bool Button::insideRect(int mouseX, int mouseY)
{
	return (mouseX > (x - width / 2.0) && mouseX < (x + width / 2.0) &&
		mouseY >(y - height / 2.0) && mouseY < (y + height / 2.0));
}

void Button::callAllCallbacks()
{
	if (m_callback != nullptr) m_callback();
	if (m_callback2 != nullptr) m_callback2(m_caller);
}

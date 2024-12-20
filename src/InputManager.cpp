#include "InputManager.h"
#include <freeglut.h>

InputManager* InputManager::s_instance = nullptr;

bool InputManager::botonesMouse[4];
glm::ivec2 InputManager::m_mousePos;

bool InputManager::mouseButtonClicked(MOUSEBUTTON button) const
{
	return botonesMouse[button];
}

glm::ivec2 InputManager::mousePos() const 
{
	return m_mousePos;
}

void InputManager::setMousePos(int x, int y)
{
	glutWarpPointer(x, y);
	m_mousePos = { x, y };
}

void InputManager::mouseKeyPressed(int button, int state, int x, int y)
{
	//std::cout << button << std::endl;
	botonesMouse[button] = !(bool)state; // state = 0 si está pulsado
}

void InputManager::mouseMotion(int x, int y)
{
	//std::cout << "Motion" << std::endl;
	m_mousePos = { x, y };
}
#include "InputManager.h"

#include <freeglut.h>

InputManager* InputManager::s_instance = nullptr;

bool InputManager::thisFrameKey = false;
bool InputManager::thisFrameSpecialKey = false;

unsigned char InputManager::m_pressedKey = NULL;
int InputManager::m_specialKey = 0;
bool InputManager::botonesMouse[5];
glm::ivec2 InputManager::m_mousePos;

void InputManager::Update()
{
	thisFrameKey = false;
	thisFrameSpecialKey = false;
	// Solo reseteamos la rueda
	botonesMouse[3] = false;
	botonesMouse[4] = false;
}

bool InputManager::getMouseKey(MOUSEBUTTON button) const
{
	return botonesMouse[button];
}

glm::ivec2 InputManager::getMousePos() const 
{
	return m_mousePos;
}

bool InputManager::getKey(unsigned char key) const
{
	return (m_pressedKey == key);
}

bool InputManager::getKeyDown(unsigned char key) const
{
	return (thisFrameKey && m_pressedKey == key);
}

bool InputManager::getSpecialKey(int key) const
{
	return (m_specialKey == key);
}

bool InputManager::getSpecialKeyDown(int key) const
{
	return (thisFrameSpecialKey && m_specialKey == key);
}

void InputManager::setMousePos(int x, int y)
{
	glutWarpPointer(x, y);
	m_mousePos = { x, y };
}

void InputManager::setCursor(int cursor)
{
	glutSetCursor(cursor);
}


// - - - - - - - - - - - - - - Callbacks de GLUT - - - - - - -

void InputManager::keyPressed(unsigned char key)
{
	m_pressedKey = key;
	thisFrameKey = true;
}


void InputManager::keyUp(unsigned char key)
{
	if(m_pressedKey == key)
		m_pressedKey = NULL;
}

void InputManager::specialKeyPressed(int key)
{
	m_specialKey = key;
	thisFrameSpecialKey = true;
}

void InputManager::specialKeyUp(int key)
{
	if (m_specialKey == key)
		m_specialKey = 0;
}

void InputManager::mouseKeyPressed(int button, int state, int x, int y)
{
	// Botones
	if (button < 3)
		botonesMouse[button] = !(bool)state; // state = 0 si está pulsado
	// Rueda
	else
		botonesMouse[button] = true;
}

void InputManager::mouseMotion(int x, int y)
{
	m_mousePos = { x, y };
}
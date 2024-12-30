#include "InputManager.h"

#include <freeglut.h>

InputManager* InputManager::s_instance = nullptr;


InputManager::InputManager()
{
	// Valores por defecto
	for (int i = 0; i < 5; i++)
		botonesMouse[i] = false;

	for (int i = 0; i < 256; i++)
		m_keys[i] = false;

	for (int i = 0; i < 256; i++)
		m_specialKeys[i] = false;

	m_mousePos = { 0,0 };
}

void InputManager::Update()
{
	m_thisFrameKeys.clear();
	m_thisFrameSpecialKeys.clear();
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
	return m_keys[key];
}

bool InputManager::getKeyDown(unsigned char key) const
{
	return (m_thisFrameKeys.find(key) != m_thisFrameKeys.end());
}

bool InputManager::getSpecialKey(int key) const
{
	return m_specialKeys[key];
}

bool InputManager::getSpecialKeyDown(int key) const
{
	return (m_thisFrameSpecialKeys.find(key) != m_thisFrameSpecialKeys.end());
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
	m_keys[key] = true;
	m_thisFrameKeys.insert(key);
}


void InputManager::keyUp(unsigned char key)
{
	m_keys[key] = false;
}

void InputManager::specialKeyPressed(int key)
{
	// Asegurarnos de que key cabe en un byte
	if (key > 0x00FF)
		std::cout << "¡Ups! El valor de la tecla se pasa del byte" << std::endl;
	m_specialKeys[key] = true;
	m_thisFrameSpecialKeys.insert(key);
}

void InputManager::specialKeyUp(int key)
{
	m_specialKeys[key] = false;
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
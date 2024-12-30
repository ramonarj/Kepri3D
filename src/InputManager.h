#ifndef __INPUTMANAGER__
#define __INPUTMANAGER__

#include <glm.hpp>
#include <iostream>
#include <set>

enum MOUSEBUTTON
{
	LEFT = 0,
	MIDDLE = 1,
	RIGHT = 2,
	WHEEL_DOWN = 3,
	WHEEL_UP = 4
};

class InputManager
{
public:
	static InputManager* Instance()
	{
		if (s_instance == nullptr)
			s_instance = new InputManager();
		return s_instance;
	}

	// GETTERS
	/* Indica si el botón del ratón está pulsado */
	bool getMouseKey(MOUSEBUTTON button) const;

	/* Devuelve la posición del ratón en la pantalla */
	glm::ivec2 getMousePos() const;

	/* Indica si la tecla está pulsada */
	bool getKey(unsigned char key) const;
	bool getKeyDown(unsigned char key) const;

	bool getSpecialKey(int key) const;
	bool getSpecialKeyDown(int key) const;

	// SETTERS
	/* Teletransporta el mouse a la posición dada */
	void setMousePos(int x, int y);

	/* Cambia el icono del mouse */
	void setCursor(int cursor);

	// Todos los callbacks para GLUT
	void keyPressed(unsigned char key);
	void keyUp(unsigned char key);

	void specialKeyPressed(int key);
	void specialKeyUp(int key);

	void mouseMotion(int x, int y);
	void mouseKeyPressed(int button, int state, int x, int y);

	void Update();

	/* Limpia la instancia; debe llamarse explícitamente */
	inline void Clean() { delete s_instance; s_instance = nullptr; }

private:
	/* Instancia y constructora privada */
	InputManager();
	static InputManager* s_instance;

	/* Posición del ratón en la pantalla (en píxeles) */
	glm::ivec2 m_mousePos;

	/* Estado de los botones del ratón */
	bool botonesMouse[5];

	/* Estado del teclado, con 256 teclas */
	bool m_keys[256];

	/* Teclas especiales de GLUT */
	bool m_specialKeys[256];

	/* Lista de teclas pulsadas en el frame actual */
	std::set<unsigned char> m_thisFrameKeys;

	// Si se ha pulsado una tecla especial en el frame actual
	std::set<int> m_thisFrameSpecialKeys;
};

#endif
#ifndef __INPUTMANAGER__
#define __INPUTMANAGER__

#include <glm.hpp>
#include <iostream>

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
	/* Indica si el bot�n del rat�n est� pulsado */
	bool getMouseKeyDown(MOUSEBUTTON button) const;

	/* Devuelve la posici�n del rat�n en la pantalla */
	glm::ivec2 getMousePos() const;

	/* Indica si la tecla est� pulsada */
	bool getKey(unsigned char key) const;
	bool getKeyDown(unsigned char key) const;

	bool getSpecialKey(int key) const;
	bool getSpecialKeyDown(int key) const;

	// SETTERS
	/* Teletransporta el mouse a la posici�n dada */
	void setMousePos(int x, int y);

	/* Cambia el icono del mouse */
	void setCursor(int cursor);

	// Todos los callbacks para GLUT
	static void keyPressed(unsigned char key);
	static void keyUp(unsigned char key);

	static void specialKeyPressed(int key);
	static void specialKeyUp(int key);

	static void mouseMotion(int x, int y);
	static void mouseKeyPressed(int button, int state, int x, int y);

	void Update();

	/* Limpia la instancia; debe llamarse expl�citamente */
	inline void Clean() { delete s_instance; s_instance = nullptr; }

private:
	/* Instancia y constructora privada */
	InputManager() {}
	static InputManager* s_instance;

	/* Posici�n del rat�n en la pantalla (en p�xeles) */
	static glm::ivec2 m_mousePos;

	/* Estado de los botones del rat�n */
	static bool botonesMouse[5];

	/* Tecla pulsada en este frame (solo una con GLUT) */
	static unsigned char m_pressedKey;

	/* Tecla especial pulsada en este frame (solo una con GLUT) */
	static int m_specialKey;

	static bool thisFrameKey;
	static bool thisFrameSpecialKey;
};

#endif
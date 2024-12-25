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


	bool mouseButtonClicked(MOUSEBUTTON button) const;

	/* Devuelve la posici�n del rat�n en la pantalla */
	glm::ivec2 mousePos() const;

	/* Teletransporta el mouse a la posici�n dada */
	void setMousePos(int x, int y);

	// Todos los callbacks para GLUT
	static void mouseMotion(int x, int y);
	static void mouseKeyPressed(int button, int state, int x, int y);


	/* Limpia la instancia; debe llamarse expl�citamente */
	inline void Clean() { delete s_instance; s_instance = nullptr; }

private:
	/* Instancia y constructora privada */
	InputManager() {}
	static InputManager* s_instance;

	/* Posici�n del rat�n en la pantalla (en p�xeles) */
	static glm::ivec2 m_mousePos;

	static bool botonesMouse[4];
};

#endif
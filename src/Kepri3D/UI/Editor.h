#ifndef __KEPRI_EDITOR__
#define __KEPRI_EDITOR__

class Game;

// Callbacks para los eventos del editor
void editorDisplay();

void accionesMenu(int accion);
void creationMenu(int eleccion);

struct Editor
{
	Editor(Game* game, int w, int h);
	
	// Métodos
	void updateMenuInfo();

	// Atributos
	int editorWindow = 0; // ID de la ventana del editor
	Game* pGame;
};


#endif

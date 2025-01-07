#ifndef __GAME__
#define __GAME__
//#include <windows.h>
//#include <gl/GL.h> // la versión incluida en Windows es muy antigua, usaremos la que incluye freeglut

// Hay que usar:
// 1) OpenGL
// 2) Open GL Mathematics (glm) -> tipos definidos, como vectores y matrices de distinto tipo
// 3) GLEW
// 4) FreeGLUT -> creación de ventanas e input del usuario (reemplazable por SDL, por ejemplo)
// Nota: FreeGLUT ya incluye OpenGL de por sí

#include <glew.h>
#include <freeglut.h>

#include "InputManager.h"
#include "checkML.h"
#include <string>

class Scene;
class Camera;
class Viewport;

/* Clase singleton para gestionar el juego */
class Game
{
public:
	/* Devuelve un puntero a la isntancia */
	static Game* Instance()
	{
		if (instance == nullptr)
			instance = new Game();
		return instance;
	}

	/* Destructora */
	~Game();

	/* Activa los subsistemas de OpenGL */
	void init(int argc, char* argv[], int windowWidth, int windowHeight, const std::string& windowName);

	/* Ejecuta el bucle principal del juego (hasta que se cierre la ventana) */
	void run();

	/* Pinta la escena activa */
	void render();

	/* Actualiza la escena activa */
	void update();

	/* Limpia la instancia */
	void clean();

	// Para callbacks
	/* Redimensiona el */
	void resize(int newWidth, int newHeight);

private:
	Game() : camera(nullptr), scene(nullptr), viewport(nullptr){}

	/* Instancia del singleton */
	static Game* instance;

	/* Escena actual */
	Scene* scene;

	/* Cámara y puerto de vista para mostrar la escena */
	Camera* camera;
	Viewport* viewport;

	/* ID de la ventana de GLUT */
	int glutWindow = 0;

	/* Nombre de la ventana, dado por el usuario */
	std::string windowName;

	/* Instante del último update */
	GLuint last_update_tick = 0;

	// - Métodos auxiliares - //
	/* Inicializa GLUT */
	void iniciarGlut(int argc, char* argv[], int windowW, int windowH);

	/* Inicializa GLEW */
	void iniciarGLEW();

	/* Activa los subsistemas de OpenGL */
	void initGLSubsystems();

	/* Registra los callbacks para los eventos de GLUT */
	void registerGlutCallbacks();
};

#endif
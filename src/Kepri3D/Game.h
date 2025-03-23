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

#define __DEBUG_INFO__

const int MSAA_SAMPLES = 4;

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

	/* Crea la ventana, inicializa OpenGL y inicializa la primera escena */
	void init(int argc, char* argv[], int windowWidth, int windowHeight, const std::string& windowName);

	/* Carga la escena inicial del juego*/
	void loadScene(Scene* scene);

	/* Sale del juego */
	void exitGame();

	/* Ejecuta el bucle principal del juego (hasta que se cierre la ventana) */
	void run();

	/* Pinta la escena activa */
	void render();

	/* Actualiza la escena activa */
	void update();

	/* Limpia la instancia */
	void clean();

	// Getters
	inline Camera* getCamera() const { return camera; }
	inline Scene* getScene() const { return scene; }

	/* Activa/desactiva un parámetro booleano de OpenGL */
	static void switchBoolParam(GLenum param);

	/* Actualiza la Scissor Box, limpiando antes a negro el/los buffers */
	static void updateScissorBox(int x, int y, int width, int height);

	// Para callbacks
	/* Redimensiona el */
	void resize(int newWidth, int newHeight);


#ifdef __DEBUG_INFO__
	struct DebugInfo
	{
		GLuint numVerts;
		GLuint numTris;
		GLuint numTextureBinds;
		GLuint fps;
		GLuint numLuces;
		GLuint numTrans;
		glm::ivec2 fbSize;
		GLuint programChanges;
	};
	DebugInfo debugInfo;
#endif

private:
	Game() : camera(nullptr), scene(nullptr), viewport(nullptr), nextScene(nullptr){}

	/* Instancia del singleton */
	static Game* instance;

	/* Escena actual */
	Scene* scene;

	/* Escena demandada */
	Scene* nextScene;

	/* Cámara y puerto de vista para mostrar la escena */
	Camera* camera;
	Viewport* viewport;

	/* ID de la ventana de GLUT */
	int glutWindow = 0;
	int editorWindow = 0;

	/* Nombre de la ventana, dado por el usuario */
	std::string windowName;

	/* Instante del último update */
	GLuint last_update_tick = 0;

#ifdef _WIN32
	/* Información sobre el contexto del dispositivo y de OpenGL */ //TODO: usar un mutex
	// Handler to the Device Context
	HDC hdc;
	// Handler to the GL Rendering Context
	HGLRC hglrc;
#endif

	// - Métodos auxiliares - //
	/* Inicializa GLUT */
	void iniciarGlut(int argc, char* argv[], int windowW, int windowH);

	/* Inicializa GLEW */
	void iniciarGLEW();

	/* Activa los subsistemas de OpenGL */
	void initGLSubsystems();

	/* Registra los callbacks para los eventos de GLUT */
	void registerGlutCallbacks();

	/* Carga la escena y quita la actual */
	void loadScenePriv(Scene* scene);

	// - - - Funciones para hilos - - - //
	void thread_loadResources();

	// EDITOR //
	void updateMenuInfo();
};

#endif
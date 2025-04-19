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

#include "Defines.h"
#include "InputManager.h"
#include "checkML.h"
#include <string>
#include <vector>

class Scene;
class Camera;
class Viewport;
struct Editor;

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

	/* Carga la escena inicial del juego */
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

	// Añade un puerto de vista
	Viewport* addViewport(int w, int h, int x, int y);
	Viewport* getViewport(int i = 0) { return m_viewports[i]; }

	// Getters
	Camera* getCamera(int i = 0) const;
	inline Scene* getScene() const { return scene; }
	glm::ivec2 getWindowSize() const;

	/* Activa/desactiva un parámetro booleano de OpenGL */
	static void switchBoolParam(GLenum param);

	/* Actualiza la Scissor Box, limpiando antes a negro el/los buffers */
	static void updateScissorBox(int x, int y, int width, int height);

	/* Activa/desactiva el Frustum Culling */
	bool getFrustrumCulling();
	void setFrustumCulling(bool b);

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
		GLuint lucesActivas;
		GLuint numTrans;
		glm::ivec2 fbSize;
		GLuint programChanges;
		GLuint culledEntities;
	};
	DebugInfo debugInfo;
#endif

private:
	friend struct Editor;
	Game() : scene(nullptr), nextScene(nullptr){}

	/* Instancia del singleton */
	static Game* instance;

	/* - - - - Ventana - - - - */
	// ID
	int glutWindow = 0;
	// Nombre
	std::string windowName;
	// Dimensiones
	GLuint windowWidth;
	GLuint windowHeight;

	/* Escena actual */
	Scene* scene;

	/* Escena demandada */
	Scene* nextScene;

	/* Puertos de vista para mostrar la escena */
	std::vector<Viewport*> m_viewports;

	/* Referencia al editor */
	Editor* pEditor;

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
};

#endif
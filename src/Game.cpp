#include "Game.h"

#include "Scene.h"
#include "Camera.h"
#include "Material.h"
#include "InputManager.h"

#include <freeglut.h>
#include <glew.h>
#include <iostream>

Game* Game::instance = nullptr;

// Predeclaraci�n de callbacks
void display();
void key(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void specialKey(int key, int x, int y);
void specialKeyUp(int key, int x, int y);
void resizeCallback(int newWidth, int newHeight);
void updateCallback();
void motion(int x, int y);
void mouse(int button, int state, int x, int y);
void clickedMotion(int x, int y);


void Game::init(int argc, char* argv[], int windowWidth, int windowHeight)
{
	// 1) Iniciar GLUT y GLEW
	iniciarGlut(argc, argv, windowWidth, windowHeight);
	iniciarGLEW();

	// Para que se inicialicen las variables est�ticas
	InputManager::Instance();

	// 2) Crear el puerto de vista, la c�mara y la escena
	viewport = new Viewport(windowWidth, windowHeight);
	camera = new Camera(viewport);
	scene = new Scene(camera);

	// 3) Registrar los distintos callbacks de teclado, rat�n y ventana
	registerGlutCallbacks();

	// 4) Iniciar subsistemas de OpenGL (texturas, luz, blending, etc)
	initGLSubsystems();

	// 5) Iniciar la escena(cargar recursos necesarios, crear entidades y colocarlas)
	scene->init();
}

void Game::run()
{
	glutMainLoop();
}

void Game::render()
{
	scene->render();
}

void Game::update()
{
	GLuint deltaTime = glutGet(GLUT_ELAPSED_TIME) - last_update_tick;
	last_update_tick = glutGet(GLUT_ELAPSED_TIME);

	//Llamamos al update y al render
	scene->update(deltaTime); 
	render();
}

void Game::clean()
{
	delete instance;
	instance = nullptr;

	// Esta funci�n acaba la ejecuci�n del programa; hay que llamarla lo �ltimo
	glutDestroyWindow(glutWindow);
}

Game::~Game()
{
	// Desactivar los par�metros de OpenGL
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	// Borrar la escena, c�mara y puerto de vista (en orden inverso)
	delete scene;
	delete camera;
	delete viewport;
}

void Game::iniciarGlut(int argc, char* argv[], int windowW, int windowH)
{
	// Inicializar GLUT y crear la ventana
	std::cout << "Starting console..." << '\n';
	glutInit(&argc, argv);

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitWindowSize(windowW, windowH);   // window size
	//glutInitWindowPosition (140, 140);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE); // | GLUT_STENCIL

	glutWindow = glutCreateWindow("Kepri3D");  // nombre de la ventana
	glutIgnoreKeyRepeat(true);
	//glutFullScreen();
}

void Game::iniciarGLEW()
{
	// Inicializar GLEW (IMPORTANTE)
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	// Informaci�n de debug
	std::cout << "OpenGL " << glGetString(GL_VERSION) << '\n';
	std::cout << glGetString(GL_VENDOR) << '\n';
}

void Game::registerGlutCallbacks()
{
	// Se llama cuando la ventana es redimensionada (tirando de los bordes/cambiando a pantalla completa)
	glutReshapeFunc(resizeCallback);
	// Se llama cuando se pulsa una tecla
	glutKeyboardFunc(key);
	// Se llama cuando se suelta una tecla
	glutKeyboardUpFunc(keyUp);
	// Se llama cuando se pulsa una tecla especial (flechas de direcci�n)
	glutSpecialFunc(specialKey);
	// Se llama cuando se pulsa una tecla especial (flechas de direcci�n)
	glutSpecialUpFunc(specialKeyUp);
	// Se llama cuando la ventana se redibuja
	glutDisplayFunc(display);
	// Se llama cada frame, aunque no se reciban eventos de la ventana (para animaciones)
	glutIdleFunc(updateCallback);
	// Se llama cuando el rat�n se mueve por la ventana sin tener ning�n bot�n pulsado
	glutPassiveMotionFunc(motion);
	// Se llama cuando se pulsa alguno de los botones del rat�n
	glutMouseFunc(mouse);
	// Se llama cuando el rat�n se mueve por la ventana teniendo pulsado alguno de los botones
	glutMotionFunc(clickedMotion);
}

void Game::initGLSubsystems()
{
	// Color de fondo (el predeterminado es negro)
	glClearColor(1.0, 1.0, 1.0, 0);  // (alpha=1 -> opaque)

	/* OpenGL basic settings */
	// Activa el Z-buffer. Si no lo activ�ramos, se pintar�a todo con el algoritmo
	// del pintor (lo m�s reciente tapa lo antiguo)
	glEnable(GL_DEPTH_TEST);

	// Activa el uso de texturas 2D
	glEnable(GL_TEXTURE_2D);

	// Activa las transparencias, indicando qu� canal usar para ello (SRC_ALPHA).
	// Si no llam�ramos a glBlendFunc, se usar�an los par�metros por defecto (0, 1) y no
	// habr�a transparencias
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //src, dest

	// Activa el descarte de fragmentos cuyo alfa no cumpla una cierta condici�n dada
	// NOTA: en el pipeline, va primero el alpha test y despu�s el blend
	//glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.15);

	/* Antialiasing; tanto para l�neas, como para pol�gonos */
	glDisable(GL_MULTISAMPLE); //desactivado por defecto
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	/* Culling; descarta el dibujado de los pol�gonos traseros.
	Lo suyo ser�a tenerlo siempre activo, y las entidades que necesiten renderizar las 2 caras,
	lo desactivan y vuelven a activar en su 'render' */
	glEnable(GL_CULL_FACE);

	/* Iluminaci�n, activa el uso de las luces y sombreado */
	glEnable(GL_LIGHTING);
	// Tipo de modelo de sombreado -> GL_FLAT (flat), GL_SMOOTH (gouraud)
	glShadeModel(GL_SMOOTH);
	Material::setShadingType(GL_SMOOTH); // importante especificarlo para materiales tambi�n

	// Para no iluminar las caras traseras de las mallas. Por defecto est� a false
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	// Punto de vista para la reflexi�n especular de los materiales
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	// Establecer la luz ambiente de toda la escena. Por defecto es (0.2, 0.2, 0.2, 1).
	GLfloat amb[4]{ 0.0, 0.0, 0.0, 1 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
	// Normalizar los vectores normales. Si hacemos bien el c�lculo de las normales en IndexMesh, no har�a ni falta.
	//glEnable(GL_NORMALIZE);
	// Usar los colores de los v�rtices
	//glEnable(GL_COLOR_MATERIAL);
}

void Game::resize(int newWidth, int newHeight)
{
	// Resize Viewport 
	viewport->setSize(newWidth, newHeight);
	// Resize Scene Visible Area 
	camera->setSize(viewport->getW(), viewport->getH());
}

// - - - - - - - - - - - - - - - - - - - - - - - - -

// Callbacks para los eventos de GLUT
void display()   // double buffer
{
	Game::Instance()->render();
}

void keyUp(unsigned char key, int x, int y)
{
	// Temporal
	InputManager::Instance()->keyUp(key);
}

void key(unsigned char key, int x, int y)
{
	// Temporal
	InputManager::Instance()->keyPressed(key);
}

void specialKey(int key, int x, int y)
{
	// Temporal
	InputManager::Instance()->specialKeyPressed(key);
}

void specialKeyUp(int key, int x, int y)
{
	// Temporal
	InputManager::Instance()->specialKeyUp(key);
}

void resizeCallback(int newWidth, int newHeight)
{
	Game::Instance()->resize(newWidth, newHeight);
}

void updateCallback()
{
	Game::Instance()->update();
}

void mouse(int button, int state, int x, int y)
{
	// Temporal
	InputManager::Instance()->mouseKeyPressed(button, state, x, y);

}

void motion(int x, int y)
{
	// Soluci�n temporal
	InputManager::Instance()->mouseMotion(x, y);
}

void clickedMotion(int x, int y)
{
	motion(x, y);
}
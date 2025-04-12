#include "Game.h"

#include "Kepri3D.h"

#include <iostream>
#include <vector>
#include <thread>

Game* Game::instance = nullptr;

// Predeclaración de callbacks
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
// - - -
void editorDisplay();

unsigned int currentSec = 0;
unsigned int fps = 0;

void Game::init(int argc, char* argv[], int windowWidth, int windowHeight, const std::string& windowName)
{
	// 1) Iniciar GLUT (la ventana) y GLEW (el contexto de OpenGL)
	iniciarGlut(argc, argv, windowWidth, windowHeight);
	iniciarGLEW();
	this->windowName = windowName;

	// Iniciar los subsistemas de input y audio
	InputManager::Instance();
	AudioManager::Instance();

	// 2) Crear el puerto de vista y la cámara
	viewport = new Viewport(windowWidth, windowHeight);
	camera = new Camera(viewport);

	// 3) Registrar los distintos callbacks de teclado, ratón y ventana
	registerGlutCallbacks();

	// 4) Iniciar subsistemas de OpenGL (texturas, luz, blending, etc)
	initGLSubsystems();

	// 5) Crear Framebuffers y asignar referencias necesarias para las escenas
	Scene::setupStatics(camera);
}

void Game::loadScene(Scene* sc)
{
	// Primera escena que cargamos (no hay ninguna ejecutándose)
	if(scene == nullptr)
	{
		// Cambiar la escena
		scene = sc;

		// Cargar recursos necesarios
#ifdef _WIN32
		// Quitar el contexto de este hilo
		assert(wglMakeCurrent(0, 0));
		// Lanzar el hilo que cargará los recursos y esperar a que acabe
		std::thread hiloRecursos(&Game::thread_loadResources, this);
		glutSetWindowTitle("Cargando escena inicial");
		hiloRecursos.join();
		// Devolver el contexto a este hilo
		assert(wglMakeCurrent(hdc, hglrc));
#else
		scene->loadResources();
		scene->init();
		scene->startComponents();
#endif
		// Crear las entidades y colocarlas
		//scene->init();

		last_update_tick = glutGet(GLUT_ELAPSED_TIME);
		std::cout << "Cargada escena '" << scene->getName() << "'" << std::endl;
		// TODO StateMachine
		pEditor->updateMenuInfo(); // editor
	}
	// Guardarnos la petición para ponerla al terminar el frame
	else
	{
		nextScene = sc;
	}
}

void Game::loadScenePriv(Scene* sc)
{
	delete scene;

	scene = sc;
	scene->loadResources();
	scene->init();
	scene->startComponents();

	last_update_tick = glutGet(GLUT_ELAPSED_TIME);
	std::cout << "Cargada escena '" << scene->getName() << "'" << std::endl;
}

void Game::thread_loadResources()
{
#ifdef _WIN32
	// Transmitir el contexto a este hilo
	assert(wglMakeCurrent(hdc, hglrc));
	// Cargar los recursos de la escena en cuestión
	scene->loadResources();
	scene->init();
	scene->startComponents();
	// Quitar el contexto
	wglMakeCurrent(0, 0);
#endif
}

void Game::exitGame()
{
	glutLeaveMainLoop();
}

void Game::run()
{
	glutMainLoop();
}

void Game::render()
{
	scene->render();

	// Recopilar información de depuración una vez hemos acabado de dibujar todo
#ifdef __DEBUG_INFO__
	debugInfo.numVerts = Mesh::numVerts;
	debugInfo.numTris = Mesh::numTris;
	debugInfo.numTextureBinds = Texture::numBinds;
	debugInfo.numLuces = Light::cont;
	debugInfo.fbSize = scene->fbSize;
	debugInfo.numTrans = scene->numberOfTrans();
	debugInfo.programChanges = Shader::programChanges;

	Mesh::numVerts = Mesh::numTris = Texture::numBinds = 0;
	Shader::programChanges = 0;
#endif
}

void Game::update()
{
	GLuint deltaTime = glutGet(GLUT_ELAPSED_TIME) - last_update_tick;
	last_update_tick = glutGet(GLUT_ELAPSED_TIME);

	//Llamamos al update y al render
	scene->update(deltaTime / 1000.0); 
	render();

	// Conteo de FPS
	currentSec += deltaTime;
	fps++;
	if(currentSec > 1000)
	{
		glutSetWindowTitle((windowName + " - " + std::to_string(fps) + " fps").c_str());
		currentSec = 0;
#ifdef __DEBUG_INFO__
		debugInfo.fps = fps;
#endif
		fps = 0;
	}

	// ¿Nos han pedido cambiar de escena?
	if (nextScene != nullptr)
	{
		glutSetWindowTitle("Cargando escena . . .");
		loadScenePriv(nextScene);
		nextScene = nullptr;
	}
}

void Game::clean()
{
	delete pEditor;

	delete instance;
	instance = nullptr;

	// Esta función acaba la ejecución del programa; hay que llamarla lo último
	if(glutGetWindow())
		glutDestroyWindow(glutWindow);
}

Game::~Game()
{
	// Desactivar los parámetros de OpenGL
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	// Borrar la escena, cámara y puerto de vista (en orden inverso)
	delete scene;
	delete camera;
	delete viewport;
	Scene::clean();
}

void Game::iniciarGlut(int argc, char* argv[], int windowW, int windowH)
{
	// Inicializar GLUT y crear la ventana
	std::cout << "Starting console..." << '\n';
	glutInit(&argc, argv);

	// Versión 3.3
	glutInitContextVersion(3, 3);
	// + 'GLUT_CORE_PROFILE' requiere que todo lo que se pinte se haga a través de VAOs 
	// + 'GLUT_COMPATIBILITY_PROFILE' permite usar VBOs independientes, paso de info CPU-GPU cada frame,
	// y todas las funciones del Fixed Pipeline
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE); 
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);


	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE | GLUT_STENCIL); // | GLUT_STENCIL
	// Número de samples
	glutSetOption(GLUT_MULTISAMPLE, MSAA_SAMPLES);

	// - - - Ventana del editor - - - //
	pEditor = new Editor(this, 200, windowH);

	// - - - Ventana del juego - - - //
	glutInitWindowSize(windowW, windowH);   // window size
	glutInitWindowPosition(250, 100);
	glutWindow = glutCreateWindow(windowName.c_str());  // nombre de la ventana

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

	// Información de debug
	std::cout << "OpenGL " << glGetString(GL_VERSION) << '\n';
	std::cout << glGetString(GL_VENDOR) << '\n';

	// Información sobre el contexto de OpenGL (Windows)
#ifdef _WIN32
	hdc = wglGetCurrentDC();
	hglrc = wglGetCurrentContext();
#endif
}

void Game::registerGlutCallbacks()
{
	// Se llama cuando la ventana es redimensionada (tirando de los bordes/cambiando a pantalla completa)
	glutReshapeFunc(resizeCallback);
	// Se llama cuando se pulsa una tecla
	glutKeyboardFunc(key);
	// Se llama cuando se suelta una tecla
	glutKeyboardUpFunc(keyUp);
	// Se llama cuando se pulsa una tecla especial (como las flechas de dirección)
	glutSpecialFunc(specialKey);
	// Se llama cuando se suelta una tecla especial
	glutSpecialUpFunc(specialKeyUp);
	// Se llama cuando la ventana se redibuja
	glutDisplayFunc(display);
	// Se llama cada frame, aunque no se reciban eventos de la ventana
	glutIdleFunc(updateCallback);
	// Se llama cuando el ratón se mueve por la ventana sin tener ningún botón pulsado
	glutPassiveMotionFunc(motion);
	// Se llama cuando se pulsa alguno de los botones del ratón
	glutMouseFunc(mouse);
	// Se llama cuando el ratón se mueve por la ventana teniendo pulsado alguno de los botones
	glutMotionFunc(clickedMotion);
}

void Game::initGLSubsystems()
{
	// Color de fondo (el predeterminado es negro)
	glClearColor(1.0, 1.0, 1.0, 0);  // (alpha=1 -> opaque)

	/* OpenGL basic settings */
	// Activa el Z-buffer. Si no lo activáramos, se pintaría todo con el algoritmo
	// del pintor (lo más reciente tapa lo antiguo)
	glEnable(GL_DEPTH_TEST);

	// Activa el uso de texturas 2D
	glEnable(GL_TEXTURE_2D);

	// Activa las transparencias, indicando qué canal usar para ello (SRC_ALPHA).
	// Si no llamáramos a glBlendFunc, se usarían los parámetros por defecto (0, 1) y no
	// habría transparencias
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //src, dest

	// Activa el descarte de fragmentos cuyo alfa no cumpla una cierta condición dada
	// NOTA: en el pipeline, va primero el alpha test y después el blend
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0);

	/* Antialiasing; tanto para líneas, como para polígonos */
	glDisable(GL_MULTISAMPLE); //desactivado por defecto
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	/* Culling; descarta el dibujado de los polígonos traseros.
	Lo suyo sería tenerlo siempre activo, y las entidades que necesiten renderizar las 2 caras,
	lo desactivan y vuelven a activar en su 'render' */
	glEnable(GL_CULL_FACE);

	/* Iluminación, activa el uso de las luces y sombreado */
	glEnable(GL_LIGHTING);
	// Tipo de modelo de sombreado -> GL_FLAT (flat), GL_SMOOTH (gouraud)
	glShadeModel(GL_SMOOTH);
	Material::setShadingType(GL_SMOOTH); // importante especificarlo para materiales también

	// Para no iluminar las caras traseras de las mallas. Por defecto está a false
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	// Punto de vista para la reflexión especular de los materiales
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	// Establecer la luz ambiente de toda la escena. Por defecto es (0.2, 0.2, 0.2, 1).
	GLfloat amb[4]{ 0.0, 0.0, 0.0, 1 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
	// Normalizar los vectores normales. Si hacemos bien el cálculo de las normales en IndexMesh, no haría ni falta.
	glEnable(GL_NORMALIZE);
	// Usar los colores de los vértices
	//glEnable(GL_COLOR_MATERIAL);

	/* Stencil Test */
	//glEnable(GL_STENCIL_TEST);
	// Indica si al dibujar algo se actualiza el Stencil Buffer (0xFF) o no (0x00)
	glStencilMask(0xFF);
	// Indica el valor que debe tener un fragmento para pasar el Stencil Test
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	// Indica qué hacer con el fragmento una vez pasa/falla los test (Stencil y Depth)
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// Orden de los tests//
	// SCISSOR - - > ALPHA - - > STENCIL - - > DEPTH - - > BLENDING - - > DITHERING - - > LOGIC OPERATIONS //

	/* Espacio de colores gamma, para corregir la curva que crean los monitores;
	los colores R,G,B son linearizados antes de usarse en el framebuffer */
	glEnable(GL_FRAMEBUFFER_SRGB);

	/* Teselación */
	// Número de vértices que conforman un parche
	glPatchParameteri(GL_PATCH_VERTICES, 4);
}

void Game::resize(int newWidth, int newHeight)
{
	// Update Scissor Box
	if (glIsEnabled(GL_SCISSOR_TEST))
	{
		// Obtener el área visible y actualizarlo en función de lo que ha crecido / disminuido la pantalla
		int rect[4];
		glGetIntegerv(GL_SCISSOR_BOX, rect);

		glm::ivec2 incr = { newWidth - viewport->getW(), newHeight - viewport->getH() };
		updateScissorBox(rect[0], rect[1], rect[2] + incr.x, rect[3] + incr.y);
	}

	// Resize Viewport 
	viewport->setSize(newWidth, newHeight);
	// Resize Scene Visible Area 
	camera->setSize(newWidth, newHeight);
	// Resize framebuffers
	scene->resize(newWidth, newHeight);
}

void Game::updateScissorBox(int x, int y, int width, int height)
{
	// Clear de los 2 buffers a negro
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	// Segundo buffer
	if (glutGet(GLUT_WINDOW_DOUBLEBUFFER) == 1)
	{
		glutSwapBuffers();
		glClear(GL_COLOR_BUFFER_BIT);
	}

	// Establecer la zona visible (en píxeles) del scissor box
	glScissor(x, y, width, height);

	// Volver a dejar el color de fondo a blanco
	glClearColor(1, 1, 1, 0);
}

void Game::switchBoolParam(GLenum param)
{
	// También se puede usar 'glIsEnabled()'
	GLboolean value;
	glGetBooleanv(param, &value);
	if (value)
		glDisable(param);
	else
		glEnable(param);
}

// - - - - - - - - - - - - - - - - - - - - - - - - -

// Callbacks para los eventos de GLUT
void display() 
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
	// Solución temporal
	InputManager::Instance()->mouseMotion(x, y);
}

void clickedMotion(int x, int y)
{
	motion(x, y);
}


// - - - - - - - - - - - - 

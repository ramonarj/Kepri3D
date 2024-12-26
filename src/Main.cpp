//#include <windows.h>
//#include <gl/GL.h> // la versión incluida en Windows es muy antigua, usaremos la que incluye freeglut

#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <glew.h>
#include <freeglut.h>

// Hay que usar:
// 1) OpenGL
// 2) Open GL Mathematics (glm) -> tipos definidos, como vectores y matrices de distinto tipo
// 3) GLEW
// 4) FreeGLUT -> creación de ventanas e input del usuario (reemplazable por SDL, por ejemplo)
// Nota: FreeGLUT ya incluye OpenGL de por sí

#include <iostream>
#include "Scene.h"
#include "Camera.h"
#include "Entity.h"
#include "Light.h"
#include "checkML.h"
#include "InputManager.h"

// Variables globales
Viewport viewport(800, 600);
// También podría hacerse con esto
//	/*int windowW = glutGet(GLUT_WINDOW_WIDTH);
//	int windowH = glutGet(GLUT_WINDOW_HEIGHT);*/
Camera camera(&viewport);
Scene scene(&camera);

// Auxiliares
GLuint last_update_tick = 0;
int glutWindow = 0;

// Predeclaración de callbacks
void display();
void key(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void specialKey(int key, int x, int y);
void specialKeyUp(int key, int x, int y);
void resize(int newWidth, int newHeight);
void update();
void motion(int x, int y);
void mouse(int button, int state, int x, int y);
void clickedMotion(int x, int y);

void iniciarGlut(int argc, char* argv[]);
void registerGlutCallbacks();

int main(int argc, char*argv[])
{
	// Detección de basura dinámica
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); 

	// 1) Inicializar GLUT y GLEW
	iniciarGlut(argc, argv);

	// Para que se inicialicen las variables estáticas
	InputManager::Instance();

	// 2) Registrar los distintos callbacks de teclado, ratón y ventana
	registerGlutCallbacks();

	// 3) Iniciar la escena (cargar recursos, crear entidades y colocarlas)
	scene.init();  

	// 4) Bucle de 'juego' (lo maneja glut)
	glutMainLoop();

	// 5) Destruir el contexto y salir
	glutDestroyWindow(glutWindow);

	return 0;
}

void iniciarGlut(int argc, char* argv[])
{
	// Inicializar GLUT y crear la ventana
	std::cout << "Starting console..." << '\n';
	glutInit(&argc, argv);

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitWindowSize(viewport.getW(), viewport.getH());   // window size
	//glutInitWindowPosition (140, 140);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); // | GLUT_MULTISAMPLE);   // | GLUT_STENCIL

	glutWindow = glutCreateWindow("Kepri3D");  // nombre de la ventana
	//glutFullScreen();

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
}

void registerGlutCallbacks()
{
	// Se llama cuando la ventana es redimensionada (tirando de los bordes/cambiando a pantalla completa)
	glutReshapeFunc(resize);
	// Se llama cuando se pulsa una tecla
	glutKeyboardFunc(key);
	// Se llama cuando se suelta una tecla
	glutKeyboardUpFunc(keyUp);
	// Se llama cuando se pulsa una tecla especial (flechas de dirección)
	glutSpecialFunc(specialKey);
	// Se llama cuando se pulsa una tecla especial (flechas de dirección)
	glutSpecialUpFunc(specialKeyUp);
	// Se llama cuando la ventana se redibuja
	glutDisplayFunc(display);
	// Se llama cada frame, aunque no se reciban eventos de la ventana (para animaciones)
	glutIdleFunc(update);
	// Se llama cuando el ratón se mueve por la ventana sin tener ningún botón pulsado
	glutPassiveMotionFunc(motion);
	// Se llama cuando se pulsa alguno de los botones del ratón
	glutMouseFunc(mouse);
	// Se llama cuando el ratón se mueve por la ventana teniendo pulsado alguno de los botones
	glutMotionFunc(clickedMotion);
}

void display()   // double buffer
{
	scene.render();
}

void keyUp(unsigned char key, int x, int y)
{
	InputManager::Instance()->keyUp(key);
}

void key(unsigned char key, int x, int y)
{
	// Temporal
	InputManager::Instance()->keyPressed(key);

	bool need_redisplay = true;

	switch(key)
	{
		float alfaValue;
	/* Activar/desactivar el uso del Z-buffer (DEPTH_TEST) */
	case 'z':
		if(glIsEnabled(GL_DEPTH_TEST))
			glDisable(GL_DEPTH_TEST);
		else
			glEnable(GL_DEPTH_TEST);
		break;
	/* Activar/desactivar el alpha test (ALPHA_TEST) */
	case 't':
		if (glIsEnabled(GL_ALPHA_TEST))
			glDisable(GL_ALPHA_TEST);
		else
			glEnable(GL_ALPHA_TEST);
		break;
	/* Hacer una foto */
	case 'f':
		scene.takePhoto();
		break;
	/* Bajar el mínimo de alfa para el ALPHA_TEST */
	case '1':
		glGetFloatv(GL_ALPHA_TEST_REF, &alfaValue);
        glAlphaFunc(GL_GREATER, alfaValue - 0.05);
		break;
	/* Subir el mínimo de alfa para el ALPHA_TEST */
	case '2':
		glGetFloatv(GL_ALPHA_TEST_REF, &alfaValue);
		glAlphaFunc(GL_GREATER, alfaValue + 0.05);
		break;
	default:
		need_redisplay = false;
		break;
	}

	// Obliga a que la ventana se vuelva a pintar
	if (need_redisplay)
		glutPostRedisplay();
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

void resize(int newWidth, int newHeight)
{
	// Resize Viewport 
	viewport.setSize(newWidth, newHeight);
	// Resize Scene Visible Area 
	camera.setSize(viewport.getW(), viewport.getH());
}

void update()
{
	GLuint deltaTime = glutGet(GLUT_ELAPSED_TIME) - last_update_tick;
	last_update_tick = glutGet(GLUT_ELAPSED_TIME);

	scene.update(deltaTime); //Llamamos al update y al render
	display();
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
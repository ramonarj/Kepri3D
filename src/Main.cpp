//#include <windows.h>
//#include <gl/GL.h> // la versi�n incluida en Windows es muy antigua, usaremos la que incluye freeglut

#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <freeglut.h>

// Hay que usar:
// 1) OpenGL
// 2) Open GL Mathematics (glm) -> tipos definidos, como vectores y matrices de distinto tipo
// 3) FreeGLUT -> creaci�n de ventanas e input del usuario (reemplazable por SDL, por ejemplo)
// Nota: FreeGLUT ya incluye OpenGL de por s�

#include <iostream>
#include "Scene.h"
#include "Camera.h"
#include "Entity.h"
#include "Light.h"
#include "checkML.h"
#include "InputManager.h"

// Variables globales
Viewport viewport(800, 600);
// Tambi�n podr�a hacerse con esto
//	/*int windowW = glutGet(GLUT_WINDOW_WIDTH);
//	int windowH = glutGet(GLUT_WINDOW_HEIGHT);*/
Camera camera(&viewport);
Scene scene(&camera);

// Auxiliares
GLuint last_update_tick = 0;
bool animationsOn = true;
bool fullscreen = false;
double velCamara = 0.4;
double velLuz = 0.25;
bool lockedMouse = true;
int moving = 0; // -1-> atr�s, 1-> adelante

// Predeclaraci�n de callbacks
void display();
void key(unsigned char key, int x, int y);
void specialKey(int key, int x, int y);
void resize(int newWidth, int newHeight);
void update();
void motion(int x, int y);
void mouse(int button, int state, int x, int y);
void clickedMotion(int x, int y);

int main(int argc, char*argv[])
{
	// Detecci�n de basura din�mica
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); 
	std::cout << "Starting console..." << '\n';

	// 1) Inicializar GLUT y crear la ventana
	glutInit(&argc, argv);

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitWindowSize(viewport.getW(), viewport.getH());   // window size
	//glutInitWindowPosition (140, 140);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); // | GLUT_MULTISAMPLE);   // | GLUT_STENCIL

	int win = glutCreateWindow("Kepri3D");  // nombre de la ventana

	// Hacer que el cursor sea invisible y moverlo al centro de la ventana
	glutSetCursor(GLUT_CURSOR_NONE);
	glutWarpPointer(viewport.getW() / 2, viewport.getH() / 2);

	//glutFullScreen(); // pantalla completa

	InputManager::Instance();
	// 2) Registrar los distintos callbacks (input) 
	// Se llama cuando la ventana es redimensionada (tirando de los bordes/cambiando a pantalla completa)
	glutReshapeFunc(resize);
	// Se llama cuando se pulsa una tecla
	glutKeyboardFunc(key);
	// Se llama cuando se pulsa una tecla especial (flechas de direcci�n)
	glutSpecialFunc(specialKey);
	// Se llama cuando la ventana se redibuja
	glutDisplayFunc(display);

	// Se llama cada frame, aunque no se reciban eventos de la ventana (para animaciones)
	glutIdleFunc(update);
	// Se llama cuando el rat�n se mueve por la ventana sin tener ning�n bot�n pulsado
	glutPassiveMotionFunc(motion);
	// Se llama cuando se pulsa alguno de los botones del rat�n
	glutMouseFunc(mouse);
	// Se llama cuando el rat�n se mueve por la ventana teniendo pulsado alguno de los botones
	glutMotionFunc(clickedMotion);


	// Informaci�n de debug
	std::cout << glGetString(GL_VERSION) << '\n';
	std::cout << glGetString(GL_VENDOR) << '\n';

	// 3) Iniciar la escena una vez creado el contexto
	scene.init();  

	// 4) Bucle de 'juego' (lo maneja glut)
	glutMainLoop();

	// 5) Destruir el contexto y salir
	glutDestroyWindow(win);  

	return 0;
}

void display()   // double buffer
{
	scene.render();
}

void key(unsigned char key, int x, int y)
{
	bool need_redisplay = true;

	switch(key)
	{
		float alfaValue;
	// Prueba de teletransporte de rat�n
	case 'c':
		//glutWarpPointer(200, 200);
		break;
	/* Tecla de escape: */ 
	case 27: 
		// Desbloquear el rat�n
		if(lockedMouse)
		{
			lockedMouse = false;
			// Podemos seleccionar el cursor que m�s nos guste!!
			glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
		}
		// Salir de la aplicaci�n
		else
			glutLeaveMainLoop();
		break;
	/* Activar/desactivar el uso del Z-buffer (DEPTH_TEST) */
	case 'z':
		if(glIsEnabled(GL_DEPTH_TEST))
			glDisable(GL_DEPTH_TEST);
		else
			glEnable(GL_DEPTH_TEST);
		break;
	/* Activar/desactivar las transparencias (BLEND) */
	case 'b':
		if (glIsEnabled(GL_BLEND))
			glDisable(GL_BLEND);
		else
			glEnable(GL_BLEND);
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
	// Pruebas con la c�mara
	case 'a':
		camera.translate({ -velCamara, 0, 0 }, LOCAL);
		break;
	case 'd':
		camera.translate({ velCamara, 0, 0 }, LOCAL);
		break;
	case 'w':
		camera.translate({ 0, 0, velCamara }, LOCAL);
		break;
	case 's':
		camera.translate({ 0, 0, -velCamara }, LOCAL);
		break;
	/* Movimiento de la luz en el eje Y */
	case '9':
		scene.getLight()->setPosition(scene.getLight()->getPosition() + glm::fvec4(0, -velLuz, 0, 0));
		break;
	case '0':
		scene.getLight()->setPosition(scene.getLight()->getPosition() + glm::fvec4(0, velLuz, 0, 0));
		break;
	// Movimiento arriba de la c�mara
	case 32: // espacio
		camera.translate({ 0, velCamara, 0 }, GLOBAL);
		break;
	/* Cambiar la perspectiva */
	case 'p':
		camera.changePerspective();
		break;
	/* Bajar el m�nimo de alfa para el ALPHA_TEST */
	case '1':
		glGetFloatv(GL_ALPHA_TEST_REF, &alfaValue);
        glAlphaFunc(GL_GREATER, alfaValue - 0.05);
		break;
	/* Subir el m�nimo de alfa para el ALPHA_TEST */
	case '2':
		glGetFloatv(GL_ALPHA_TEST_REF, &alfaValue);
		glAlphaFunc(GL_GREATER, alfaValue + 0.05);
		break;
	/* Enter: activar/desactivar animaciones */
	case 13:
		animationsOn = !animationsOn;
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
	bool need_redisplay = true;

	switch (key) {
	/* Movimiento de la luz en el plano XZ */
	case GLUT_KEY_RIGHT:
		scene.getLight()->setPosition(scene.getLight()->getPosition() + glm::fvec4(velLuz, 0, 0, 0));
		break;
	case GLUT_KEY_LEFT:
		scene.getLight()->setPosition(scene.getLight()->getPosition() + glm::fvec4(-velLuz, 0, 0, 0));
		break;
	case GLUT_KEY_UP:
		scene.getLight()->setPosition(scene.getLight()->getPosition() + glm::fvec4(0, 0, -velLuz, 0));
		break;
	case GLUT_KEY_DOWN:
		scene.getLight()->setPosition(scene.getLight()->getPosition() + glm::fvec4(0, 0, velLuz, 0));
		break;
	/* Movimiento arriba/abajo de la c�mara */
	case GLUT_KEY_SHIFT_L:
		camera.translate({ 0, -velCamara, 0 }, GLOBAL);
		break;
	/* Modo pantalla completa (entrar/salir) */
	case GLUT_KEY_F11:
		fullscreen = !fullscreen;
		if(fullscreen)
		{
			glutFullScreen();
			
		}
		else
		{
			glutReshapeWindow(800, 600);
		}
			
		break;
	default:
		need_redisplay = false;
		break;
	}//switch

	if (need_redisplay)
		glutPostRedisplay();
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
	// Movimiento de la c�mara
	if (moving == 1)
		camera.translate({ 0, 0, velCamara / 50 }, LOCAL);
	else if (moving == -1)
		camera.translate({ 0, 0, -velCamara / 50 }, LOCAL);

	// Animaciones de la escena
	if(animationsOn)
	{
		scene.update(deltaTime); //Llamamos al update y al render
		display();
	}
}

void mouse(int button, int state, int x, int y)
{
	// Temporal
	InputManager::Instance()->mouseKeyPressed(button, state, x, y);


	 // Se encarga la escena
	if (!lockedMouse)
	{
		lockedMouse = true;
		glutSetCursor(GLUT_CURSOR_NONE);
		return;
	}

	// Clic izquierdo
	if (button == 0)
	{
		if (state == 0)
			moving = 1;
		else if (state == 1)
			moving = 0;
	}

	// Clic derecho
	else if (button == 2)
	{
		if (state == 0)
			moving = -1;
		else if (state == 1)
			moving = 0;
	}
	// Rueda del rat�n
	if (button == 3)
	{
		camera.rotate(0.05, { 0, 0, 1 }, LOCAL);
	}
	else if (button == 4)
	{
		camera.rotate(-0.05, { 0, 0, 1 }, LOCAL);
	}
}

void motion(int x, int y)
{
	// Soluci�n temporal
	InputManager::Instance()->mouseMotion(x, y);

	if (!lockedMouse)
		return;

	// Incremento en la posici�n del rat�n
	glm::dvec2 diff((double)x - viewport.getW() / 2, (double)y - viewport.getH() / 2);

	// C�mara tipo FPS; las rotaciones en Y son globales y en X son locales.
	camera.rotate(-diff.x * 0.002, { 0,1,0 }, GLOBAL);
	camera.rotate(-diff.y * 0.002, { 1,0,0 }, LOCAL); // lo mismo que hacer pitch
	//camera.yaw(-diff.x * 0.002);
	//camera.pitch(-diff.y * 0.002);

	// Volver a dejar el rat�n en el centro
	InputManager::Instance()->setMousePos(viewport.getW() / 2, viewport.getH() / 2);

	// Repintar
	//glutPostRedisplay();
}


void clickedMotion(int x, int y)
{
	motion(x, y);
}